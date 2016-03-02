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
#include <unistd.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/ndelay.hpp>
#include <dromozoa/pipe.hpp>

namespace dromozoa {
#ifdef HAVE_PIPE2
  int compat_pipe2(int fd[2], int flags) {
    return pipe2(fd, flags);
  }
#else
  int compat_pipe2(int fd[2], int flags) {
    if (pipe(fd) == -1) {
      return -1;
    }
    do {
      if (flags & O_CLOEXEC) {
        if (coe(fd[0]) == -1) {
          break;
        }
        if (coe(fd[1]) == -1) {
          break;
        }
      }
      if (flags & O_NONBLOCK) {
        if (ndelay_on(fd[0]) == -1) {
          break;
        }
        if (ndelay_on(fd[1]) == -1) {
          break;
        }
      }
      return 0;
    } while (false);
    close_pipe(fd);
    return -1;
  }
#endif

  void close_pipe(int fd[2]) {
    int save = errno;
    close(fd[0]);
    close(fd[1]);
    fd[0] = -1;
    fd[1] = -1;
    errno = save;
  }
}
