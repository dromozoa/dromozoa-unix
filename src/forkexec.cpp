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

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <vector>

#include "pathexec.hpp"

namespace dromozoa {
  inline int coe(int fd) {
    int result = fcntl(fd, F_GETFD);
    if (result == -1) {
      return -1;
    }
    return fcntl(fd, F_SETFD, result | FD_CLOEXEC);
  }

  inline int coe_pipe(int fd[2]) {
    if (pipe(fd) == -1) {
      return -1;
    }
    if (coe(fd[0])) {
      return -1;
    }
    return coe(fd[1]);
  }

  inline void die(int die_fd[2]) {
    int code = errno;
    write(die_fd[1], &code, sizeof(code));
    _exit(1);
  }

  inline int read_die(int die_fd[2]) {
    int code = 0;
    close(die_fd[1]);
    read(die_fd[0], &code, sizeof(code));
    close(die_fd[2]);
    return code;
  }

  inline void forkexec_impl(
      const char* path,
      const char* const* argv,
      const char* const* envp,
      const char* chdir,
      const int* dup2_fd,
      bool dup2_null,
      int die_fd[2],
      std::vector<char>& buffer) {
    if (chdir) {
      if (::chdir(chdir) == -1) {
        die(die_fd);
      }
    }

    if (dup2_fd) {
      if (dup2_fd[0] != -1) {
        if (dup2(dup2_fd[0], 0) == -1) {
          die(die_fd);
        }
      }
      if (dup2_fd[1] != -1) {
        if (dup2(dup2_fd[1], 1) == -1) {
          die(die_fd);
        }
      }
      if (dup2_fd[2] != -1) {
        if (dup2(dup2_fd[2], 2) == -1) {
          die(die_fd);
        }
      }
    }

    if (dup2_null) {
      int fd = open("/dev/null", O_RDWR, 0);
      if (fd == -1) {
        die(die_fd);
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
    }

    sigset_t mask;
    if (sigemptyset(&mask) == -1) {
      die(die_fd);
    }
    if (pthread_sigmask(SIG_SETMASK, &mask, 0) == -1) {
      die(die_fd);
    }

    pathexec(path, argv, envp, buffer);
    die(die_fd);
  }

  pid_t forkexec(
      const char* path,
      const char* const* argv,
      const char* const* envp,
      const char* chdir,
      const int* dup2_fd) {
    int die_fd[] = { -1, -1 };
    if (coe_pipe(die_fd) == -1) {
      return -1;
    }
    std::vector<char> buffer(pathexec_buffer_size(path, argv));
    pid_t pid = fork();
    if (pid == -1) {
      return -1;
    } else if (pid == 0) {
      forkexec_impl(path, argv, envp, chdir, dup2_fd, false, die_fd, buffer);
    }
    int code = read_die(die_fd);
    if (code == 0) {
      return pid;
    } else {
      int status = 0;
      waitpid(pid, &status, 0);
      // _exit(1)
      errno = code;
      return -1;
    }
  }

  pid_t forkexec_daemon(
      const char* path,
      const char* const* argv,
      const char* const* envp,
      const char* chdir) {
    int die_fd[] = { -1, -1 };
    if (coe_pipe(die_fd) == -1) {
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
        forkexec_impl(path, argv, envp, chdir, 0, true, die_fd, buffer);
      }
      _exit(0);
    }
    int code = read_die(die_fd);
    if (code == 0) {
      return pid;
    } else {
      int status = 0;
      waitpid(pid, &status, 0);
      // _exit(1)
      errno = code;
      return -1;
    }
  }
}
