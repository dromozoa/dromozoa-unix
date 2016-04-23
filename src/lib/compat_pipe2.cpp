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

#include <fcntl.h>
#include <unistd.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/ndelay.hpp>
#include <dromozoa/sigmask.hpp>

namespace dromozoa {
#ifdef HAVE_PIPE2
  int compat_pipe2(int pipe_fd[2], int flags) {
    return pipe2(pipe_fd, flags);
  }
#else
  int compat_pipe2(int pipe_fd[2], int flags) {
    sigset_t mask;
    if (sigmask_block_all_signals(&mask)) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    int fd[2] = { -1, -1 };
    if (pipe(fd) == -1) {
      return -1;
    }
    file_descriptor fd0(fd[0]);
    file_descriptor fd1(fd[1]);

    if (flags & O_CLOEXEC) {
      if (coe(fd0.get()) == -1) {
        return -1;
      }
      if (coe(fd1.get()) == -1) {
        return -1;
      }
    }
    if (flags & O_NONBLOCK) {
      if (ndelay_on(fd0.get()) == -1) {
        return -1;
      }
      if (ndelay_on(fd1.get()) == -1) {
        return -1;
      }
    }

    pipe_fd[0] = fd0.release();
    pipe_fd[1] = fd1.release();
    return 0;
  }
#endif
}
