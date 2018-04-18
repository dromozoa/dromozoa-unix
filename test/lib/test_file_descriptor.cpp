// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include <unistd.h>

#include <iostream>

#include <dromozoa/file_descriptor.hpp>

int main(int, char*[]) {
  int fd[2] = { -1, -1 };
  assert(pipe(fd) != -1);
  std::cout << fd[0] << ", " << fd[1] << "\n";
  dromozoa::file_descriptor fd0(fd[0]);
  dromozoa::file_descriptor fd1;
  assert(fd0.get() == fd[0]);
  assert(fd0.valid());
  assert(fd1.get() == -1);
  assert(!fd1.valid());

  dromozoa::file_descriptor(fd[1]).swap(fd1);
  assert(fd1.get() == fd[1]);
  assert(fd1.valid());

  assert(fd0.release() == fd[0]);
  assert(fd0.get() == -1);
  assert(!fd0.valid());

  assert(close(fd[0]) != -1);
  assert(close(fd[1]) != -1);

  // unexpected: fd1 already closed
  return 0;
}
