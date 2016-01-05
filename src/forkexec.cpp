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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <iostream>
#include <vector>

#include "error.hpp"
#include "forkexec.hpp"
#include "log_level.hpp"
#include "pipe.hpp"
#include "pathexec.hpp"

namespace dromozoa {
  namespace {
#ifdef HAVE_PTHREAD
    int signal_mask(int how, const sigset_t* new_mask, sigset_t* old_mask) {
      return pthread_sigmask(how, new_mask, old_mask);
    }
#else
    int signal_mask(int how, const sigset_t* new_mask, sigset_t* old_mask) {
      return sigprocmask(how, new_mask, old_mask);
    }
#endif

    class scoped_signal_mask {
    public:
      scoped_signal_mask() : masked_() {}

      ~scoped_signal_mask() {
        if (masked_) {
          if (signal_mask(SIG_SETMASK, &mask_, 0) == -1) {
            int code = errno;
            if (get_log_level() > 0) {
              std::cerr << "[dromozoa-unix] cannot reset signal mask: ";
              print_error(std::cerr, code);
              std::cerr << std::endl;
            }
          }
        }
      }

      int mask_all_signals() {
        sigset_t mask;
        if (sigfillset(&mask) == -1) {
          return -1;
        }
        if (signal_mask(SIG_SETMASK, &mask, &mask_) == -1) {
          return -1;
        }
        masked_ = true;
        return 0;
      }

    private:
      bool masked_;
      sigset_t mask_;

      scoped_signal_mask(const scoped_signal_mask&);
      scoped_signal_mask& operator=(const scoped_signal_mask&);
    };

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

    pid_t forkexec(
        const char* path,
        const char* const* argv,
        const char* const* envp,
        const char* chdir,
        const int* dup2_stdio) {
      scoped_signal_mask scoped_mask;
      if (scoped_mask.mask_all_signals() == -1) {
        return -1;
      }
      int die_fd[] = { -1, -1 };
      if (pipe2(die_fd, O_CLOEXEC) == -1) {
        return -1;
      }
      std::vector<char> buffer(pathexec_buffer_size(path, argv));
      pid_t pid = fork();
      if (pid == -1) {
        return -1;
      } else if (pid == 0) {
        forkexec(path, argv, envp, chdir, dup2_stdio, false, die_fd, &buffer[0], buffer.size());
      }
      int code = read_die(die_fd);
      if (code == 0) {
        return pid;
      } else {
        int status;
        waitpid(pid, &status, 0);
        errno = code;
        return -1;
      }
    }

    pid_t forkexec_daemon(
        const char* path,
        const char* const* argv,
        const char* const* envp,
        const char* chdir) {
      scoped_signal_mask scoped_mask;
      if (scoped_mask.mask_all_signals() == -1) {
        return -1;
      }
      int die_fd[] = { -1, -1 };
      if (pipe2(die_fd, O_CLOEXEC) == -1) {
        return -1;
      }
      std::vector<char> buffer(pathexec_buffer_size(path, argv));
      pid_t pid = fork();
      if (pid == -1) {
        return -1;
      } else if (pid == 0) {
        if (setsid() == -1) {
          die(die_fd);
        }
        pid_t pid = fork();
        if (pid == -1) {
          die(die_fd);
        } else if (pid == 0) {
          forkexec(path, argv, envp, chdir, 0, true, die_fd, &buffer[0], buffer.size());
        }
        _exit(0);
      }
      int code = read_die(die_fd);
      if (code == 0) {
        return pid;
      } else {
        int status;
        waitpid(pid, &status, 0);
        errno = code;
        return -1;
      }
    }

    int impl_forkexec(lua_State* L) {
      return 0;
    }
  }

  void initialize_forkexec(lua_State* L) {
  }
}
