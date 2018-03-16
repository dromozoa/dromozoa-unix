// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/selfpipe.hpp>

namespace dromozoa {
  namespace {
    int selfpipe_reader = -1;
    int selfpipe_writer = -1;
    struct sigaction selfpipe_save_sa;
  }
}

extern "C" void dromozoa_selfpipe_trigger(int) {
  write(dromozoa::selfpipe_writer, "", 1);
}

namespace dromozoa {
  int selfpipe_open() {
    if (selfpipe_valid()) {
      return 0;
    }

    int fd[2] = { -1, -1 };
    if (compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
      return -1;
    }
    file_descriptor reader(fd[0]);
    file_descriptor writer(fd[1]);

    struct sigaction sa = {};
    sa.sa_handler = dromozoa_selfpipe_trigger;
    if (sigaction(SIGCHLD, &sa, &selfpipe_save_sa) == -1) {
      return -1;
    }

    selfpipe_reader = reader.release();
    selfpipe_writer = writer.release();
    return 0;
  }

  int selfpipe_close() {
    if (!selfpipe_valid()) {
      return 0;
    }

    file_descriptor reader(selfpipe_reader);
    file_descriptor writer(selfpipe_writer);
    selfpipe_reader = -1;
    selfpipe_writer = -1;

    if (sigaction(SIGCHLD, &selfpipe_save_sa, 0) == -1) {
      return -1;
    }
    if (reader.close() == -1) {
      return -1;
    }
    if (writer.close() == -1) {
      return -1;
    }
    return 0;
  }

  bool selfpipe_valid() {
    return selfpipe_reader != -1;
  }

  int selfpipe_get() {
    return selfpipe_reader;
  }

  int selfpipe_read() {
    int count = 0;
    char c;
    while (read(selfpipe_reader, &c, 1) == 1) {
      ++count;
    }
    return count;
  }
}
