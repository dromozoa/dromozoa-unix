// Copyright (C) 2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <iostream>

#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/selector.hpp>

#if defined(HAVE_EPOLL_CREATE) || defined(HAVE_EPOLL_CREATE1)
#include <dromozoa/selector_epoll.hpp>
typedef dromozoa::selector_epoll selector_impl;
#elif defined(HAVE_KQUEUE)
#include <dromozoa/selector_kqueue.hpp>
typedef dromozoa::selector_kqueue selector_impl;
#endif

#include "assert.hpp"

void test() {
  int selector_fd = selector_impl::open(dromozoa::SELECTOR_CLOEXEC);
  assert(selector_fd != -1);
  selector_impl selector(selector_fd);

  int pipe_fd[2] = { -1,-1 };
  assert(dromozoa::compat_pipe2(pipe_fd, O_CLOEXEC) != -1);

  struct timespec timeout = {};
  timeout.tv_nsec = 100000000;

  assert(selector.add(pipe_fd[0], dromozoa::SELECTOR_READ) == 0);
  assert(selector.select(&timeout) == 0);
  assert(selector.del(pipe_fd[0]) == 0);

  assert(write(pipe_fd[1], "", 1) == 1);
  assert(close(pipe_fd[1]) != -1);

  int result = -1;
  result = selector.add(pipe_fd[0], dromozoa::SELECTOR_READ);
  assert(result == 0 || errno == EPIPE);

  assert(selector.select(&timeout) == 1);

  result = selector.del(pipe_fd[0]);
  assert(result == 0 || errno == ENOENT);

  assert(selector.select(&timeout) == 0);

  assert(close(pipe_fd[0]) != -1);
}

int main(int, char*[]) {
  test();
  return 0;
}
