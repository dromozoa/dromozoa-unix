// Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-unix.
//
// dromozoa-unix is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-unix is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-unix.  If not, see <http://www.gnu.org/licenses/>.

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>

#include <vector>

#include "argument_vector.hpp"
#include "error.hpp"
#include "fd.hpp"
#include "forkexec.hpp"
#include "function.hpp"
#include "pipe.hpp"
#include "pathexec.hpp"
#include "signal_mask.hpp"

namespace dromozoa {
  namespace {
    void die(int die_fd[2]) {
      int code = errno;
      write(die_fd[1], &code, sizeof(code));
      _exit(1);
    }

    int read_die(int die_fd[2]) {
      int code = 0;
      close(die_fd[1]);
      read(die_fd[0], &code, sizeof(code));
      close(die_fd[0]);
      return code;
    }

    pid_t read_pid(int pid_fd[2]) {
      pid_t pid = -1;
      close(pid_fd[1]);
      read(pid_fd[0], &pid, sizeof(pid));
      close(pid_fd[0]);
      return pid;
    }

    void forkexec(
        const char* path,
        const char* const* argv,
        const char* const* envp,
        const char* chdir,
        const int* dup2_stdio,
        bool dup2_null,
        int die_fd[2],
        char* buffer,
        size_t size) {
      if (chdir) {
        if (::chdir(chdir) == -1) {
          die(die_fd);
        }
      }

      if (dup2_stdio) {
        if (dup2_stdio[0] != -1) {
          if (dup2(dup2_stdio[0], 0) == -1) {
            die(die_fd);
          }
        }
        if (dup2_stdio[1] != -1) {
          if (dup2(dup2_stdio[1], 1) == -1) {
            die(die_fd);
          }
        }
        if (dup2_stdio[2] != -1) {
          if (dup2(dup2_stdio[2], 2) == -1) {
            die(die_fd);
          }
        }
      }

      if (dup2_null) {
        int fd = open("/dev/null", O_RDWR, 0);
        if (fd == -1) {
          die(die_fd);
        }
        if (dup2(fd, 0) == -1) {
          die(die_fd);
        }
        if (dup2(fd, 1) == -1) {
          die(die_fd);
        }
        if (dup2(fd, 2) == -1) {
          die(die_fd);
        }
        close(fd);
      }

      sigset_t mask;
      if (sigemptyset(&mask) == -1) {
        die(die_fd);
      }
      if (signal_mask(SIG_SETMASK, &mask, 0) == -1) {
        die(die_fd);
      }

      pathexec(path, argv, envp, buffer, size);
      die(die_fd);
    }

    int forkexec(
        const char* path,
        const char* const* argv,
        const char* const* envp,
        const char* chdir,
        const int* dup2_stdio,
        pid_t& pid) {
      pid = -1;

      std::vector<char> buffer(pathexec_buffer_size(path, argv));

      scoped_signal_mask scoped_mask;
      if (scoped_mask.block_all_signals() == -1) {
        return -1;
      }
      int die_fd[] = { -1, -1 };
      if (wrap_pipe2(die_fd, O_CLOEXEC) == -1) {
        return -1;
      }

      pid = fork();
      if (pid == -1) {
        close_pipe(die_fd);
        return -1;
      } else if (pid == 0) {
        forkexec(path, argv, envp, chdir, dup2_stdio, false, die_fd, &buffer[0], buffer.size());
      }

      int code = read_die(die_fd);
      if (code == 0) {
        return 0;
      } else {
        errno = code;
        return -1;
      }
    }

    int forkexec_daemon(
        const char* path,
        const char* const* argv,
        const char* const* envp,
        const char* chdir,
        pid_t& pid1,
        pid_t& pid2) {
      pid1 = -1;
      pid2 = -1;

      std::vector<char> buffer(pathexec_buffer_size(path, argv));

      scoped_signal_mask scoped_mask;
      if (scoped_mask.block_all_signals() == -1) {
        return -1;
      }
      int die_fd[] = { -1, -1 };
      if (wrap_pipe2(die_fd, O_CLOEXEC) == -1) {
        return -1;
      }
      int pid_fd[] = { -1, -1 };
      if (wrap_pipe2(pid_fd, O_CLOEXEC) == -1) {
        close_pipe(die_fd);
        return -1;
      }

      pid1 = fork();
      if (pid1 == -1) {
        close_pipe(die_fd);
        close_pipe(pid_fd);
        return -1;
      } else if (pid1 == 0) {
        if (setsid() == -1) {
          die(die_fd);
        }
        pid_t pid = fork();
        if (pid == -1) {
          die(die_fd);
        } else if (pid == 0) {
          forkexec(path, argv, envp, chdir, 0, true, die_fd, &buffer[0], buffer.size());
        }
        write(pid_fd[1], &pid, sizeof(pid));
        _exit(0);
      }

      int code = read_die(die_fd);
      pid2 = read_pid(pid_fd);
      if (code == 0) {
        return 0;
      } else {
        errno = code;
        return -1;
      }
    }

    int impl_forkexec(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      argument_vector argv(L, 2);
      argument_vector envp(L, 3);
      const char* chdir = lua_tostring(L, 4);
      int dup2_stdio[3] = { -1, -1, -1 };
      for (int i = 0; i < 3; ++i) {
        lua_pushinteger(L, i);
        lua_gettable(L, 5);
        if (!lua_isnil(L, -1)) {
          dup2_stdio[i] = get_fd(L, -1);
        }
        lua_pop(L, 1);
      }
      pid_t pid = -1;
      if (forkexec(path, argv.get(), envp.get(), chdir, dup2_stdio, pid) == -1) {
        int result = push_error(L);
        lua_pushinteger(L, pid);
        return result + 1;
      } else {
        lua_pushinteger(L, pid);
        return 1;
      }
    }

    int impl_forkexec_daemon(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      argument_vector argv(L, 2);
      argument_vector envp(L, 3);
      const char* chdir = lua_tostring(L, 4);
      pid_t pid1 = -1;
      pid_t pid2 = -1;
      if (forkexec_daemon(path, argv.get(), envp.get(), chdir, pid1, pid2) == -1) {
        int result = push_error(L);
        lua_pushinteger(L, pid1);
        lua_pushinteger(L, pid2);
        return result + 2;
      } else {
        lua_pushinteger(L, pid1);
        lua_pushinteger(L, pid2);
        return 2;
      }
    }
  }

  void initialize_forkexec(lua_State* L) {
    function<impl_forkexec>::set_field(L, "forkexec");
    function<impl_forkexec_daemon>::set_field(L, "forkexec_daemon");
  }
}
