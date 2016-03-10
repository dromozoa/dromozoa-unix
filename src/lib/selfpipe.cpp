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

#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/pipe.hpp>
#include <dromozoa/selfpipe.hpp>

static int dromozoa_selfpipe_reader = -1;
static int dromozoa_selfpipe_writer = -1;

extern "C" void dromozoa_selfpipe_trigger(int) {
  write(dromozoa_selfpipe_writer, "", 1);
}

namespace dromozoa {
  namespace {
    struct sigaction save_sa;
  }

  int selfpipe_open() {
    if (selfpipe_valid()) {
      errno = 0;
      return -1;
    }

    int fd[2] = { -1, -1 };
    if (compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
      return -1;
    }
    file_descriptor fd0(fd[0]);
    file_descriptor fd1(fd[1]);

    struct sigaction sa = {};
    sa.sa_handler = dromozoa_selfpipe_trigger;
    if (sigaction(SIGCHLD, &sa, &save_sa) == -1) {
      return -1;
    }

    dromozoa_selfpipe_reader = fd0.release();
    dromozoa_selfpipe_writer = fd1.release();
    return 0;
  }

  int selfpipe_close() {
    if (!selfpipe_valid()) {
      errno = 0;
      return -1;
    }

    file_descriptor fd0(dromozoa_selfpipe_reader);
    file_descriptor fd1(dromozoa_selfpipe_writer);
    dromozoa_selfpipe_reader = -1;
    dromozoa_selfpipe_writer = -1;

    if (sigaction(SIGCHLD, &save_sa, 0) == -1) {
      return -1;
    }
    if (fd0.close() == -1) {
      return -1;
    }
    if (fd1.close() == -1) {
      return -1;
    }
    return 0;
  }

  bool selfpipe_valid() {
    return dromozoa_selfpipe_reader != -1;
  }

  int selfpipe_get() {
    return dromozoa_selfpipe_reader;
  }

  int selfpipe_read() {
    int count = 0;
    char c;
    while (read(dromozoa_selfpipe_reader, &c, 1) == 1) {
      ++count;
    }
    return count;
  }
}
