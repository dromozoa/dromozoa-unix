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

#include <sys/file.h>

#include <dromozoa/lock.hpp>

namespace dromozoa {
  int lock_ex(int fd) {
    return flock(fd, LOCK_EX);
  }

  int lock_exnb(int fd) {
    return flock(fd, LOCK_EX | LOCK_NB);
  }

  int lock_un(int fd) {
    return flock(fd, LOCK_UN);
  }
}
