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

#include <dromozoa/coe.hpp>

namespace dromozoa {
  int coe(int fd) {
    int result = fcntl(fd, F_GETFD);
    if (result == -1) {
      return -1;
    } else {
      return fcntl(fd, F_SETFD, result | FD_CLOEXEC);
    }
  }

  int is_coe(int fd) {
    int result = fcntl(fd, F_GETFD);
    if (result == -1) {
      return -1;
    } else {
      if (result & FD_CLOEXEC) {
        return 1;
      } else {
        return 0;
      }
    }
  }
}
