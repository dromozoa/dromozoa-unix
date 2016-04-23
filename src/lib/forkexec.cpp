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
#include <unistd.h>

#include <vector>

#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/forkexec.hpp>
#include <dromozoa/pathexec.hpp>
#include <dromozoa/sigmask.hpp>

namespace dromozoa {
  namespace {
    class forkexec_impl {
    public:
      int open_die() {
        int die_fd[] = { -1, -1 };
        if (compat_pipe2(die_fd, O_CLOEXEC) == -1) {
          return -1;
        }
        file_descriptor(die_fd[0]).swap(die_fd0_);
        file_descriptor(die_fd[1]).swap(die_fd1_);
        return 0;
      }

      void close_die_reader() {
        die_fd0_.close();
      }

      void close_die_writer() {
        die_fd1_.close();
      }

      void die() {
        int code = errno;
        write(die_fd1_.get(), &code, sizeof(code));
        this->~forkexec_impl();
        _exit(1);
      }

      int read_die() {
        int code = 0;
        read(die_fd0_.get(), &code, sizeof(code));
        die_fd0_.close();
        return code;
      }

      int open_pid() {
        int pid_fd[] = { -1, -1 };
        if (compat_pipe2(pid_fd, O_CLOEXEC) == -1) {
          return -1;
        }
        file_descriptor(pid_fd[0]).swap(pid_fd0_);
        file_descriptor(pid_fd[1]).swap(pid_fd1_);
        return 0;
      }

      int close_pid_reader() {
        return pid_fd0_.close();
      }

      int close_pid_writer() {
        return pid_fd1_.close();
      }

      void quit(pid_t pid) {
        write(pid_fd1_.get(), &pid, sizeof(pid));
        this->~forkexec_impl();
        _exit(0);
      }

      pid_t read_pid() {
        pid_t pid = -1;
        read(pid_fd0_.get(), &pid, sizeof(pid));
        pid_fd0_.close();
        return pid;
      }

      void forkexec(
          const char* path,
          const char* const* argv,
          const char* const* envp,
          const char* chdir,
          const int* dup2_stdio,
          bool dup2_null,
          char* buffer,
          size_t size) {
        if (chdir) {
          if (::chdir(chdir) == -1) {
            die();
          }
        }

        if (dup2_stdio) {
          if (dup2_stdio[0] != -1) {
            if (dup2(dup2_stdio[0], 0) == -1) {
              die();
            }
          }
          if (dup2_stdio[1] != -1) {
            if (dup2(dup2_stdio[1], 1) == -1) {
              die();
            }
          }
          if (dup2_stdio[2] != -1) {
            if (dup2(dup2_stdio[2], 2) == -1) {
              die();
            }
          }
        }

        if (dup2_null) {
          file_descriptor(open("/dev/null", O_RDWR | O_CLOEXEC, 0)).swap(null_fd_);
          if (!null_fd_.valid()) {
            die();
          }
          if (dup2(null_fd_.get(), 0) == -1) {
            die();
          }
          if (dup2(null_fd_.get(), 1) == -1) {
            die();
          }
          if (dup2(null_fd_.get(), 2) == -1) {
            die();
          }
          null_fd_.close();
        }

        if (sigmask_unblock_all_signals(0) == -1) {
          die();
        }

        pathexec(path, argv, envp, buffer, size);
        die();
      }

    private:
      file_descriptor die_fd0_;
      file_descriptor die_fd1_;
      file_descriptor pid_fd0_;
      file_descriptor pid_fd1_;
      file_descriptor null_fd_;
    };
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
    forkexec_impl forkexec_impl;

    sigset_t mask;
    if (sigmask_block_all_signals(&mask) == -1) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    if (forkexec_impl.open_die() == -1) {
      return -1;
    }

    pid = fork();
    if (pid == -1) {
      return -1;
    } else if (pid == 0) {
      forkexec_impl.close_die_reader();
      forkexec_impl.forkexec(path, argv, envp, chdir, dup2_stdio, false, &buffer[0], buffer.size());
    }
    forkexec_impl.close_die_writer();

    int code = forkexec_impl.read_die();
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
    forkexec_impl forkexec_impl;

    sigset_t mask;
    if (sigmask_block_all_signals(&mask) == -1) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    if (forkexec_impl.open_die() == -1) {
      return -1;
    }
    if (forkexec_impl.open_pid() == -1) {
      return -1;
    }

    pid1 = fork();
    if (pid1 == -1) {
      return -1;
    } else if (pid1 == 0) {
      forkexec_impl.close_die_reader();
      forkexec_impl.close_pid_reader();
      if (setsid() == -1) {
        forkexec_impl.die();
      }
      pid_t pid = fork();
      if (pid == -1) {
        forkexec_impl.die();
      } else if (pid == 0) {
        forkexec_impl.close_pid_writer();
        forkexec_impl.forkexec(path, argv, envp, chdir, 0, true, &buffer[0], buffer.size());
      }
      forkexec_impl.quit(pid);
    }
    forkexec_impl.close_die_writer();
    forkexec_impl.close_pid_writer();

    int code = forkexec_impl.read_die();
    pid2 = forkexec_impl.read_pid();
    if (code == 0) {
      return 0;
    } else {
      errno = code;
      return -1;
    }
  }
}
