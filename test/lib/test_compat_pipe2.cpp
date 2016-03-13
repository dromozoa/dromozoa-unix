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

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include <dromozoa/compat_pipe2.hpp>

void assert_coe_and_ndelay_on(int fd) {
  assert(fcntl(fd, F_GETFD) & FD_CLOEXEC);
  assert(fcntl(fd, F_GETFL) & O_NONBLOCK);
}

void assert_coe_and_ndelay_off(int fd) {
  assert(fcntl(fd, F_GETFD) & FD_CLOEXEC);
  assert(!(fcntl(fd, F_GETFL) & O_NONBLOCK));
}

int main(int, char*[]) {
  int fd[2] = { -1, -1 };

  assert(dromozoa::compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) != -1);
  std::cout << fd[0] << ", " << fd[1] << "\n";
  assert_coe_and_ndelay_on(fd[0]);
  assert_coe_and_ndelay_on(fd[1]);
  assert(close(fd[0]) != -1);
  assert(close(fd[1]) != -1);

  assert(dromozoa::compat_pipe2(fd, O_CLOEXEC) != -1);
  assert_coe_and_ndelay_off(fd[0]);
  assert_coe_and_ndelay_off(fd[1]);
  assert(close(fd[0]) != -1);
  assert(close(fd[1]) != -1);

  return 0;
}
