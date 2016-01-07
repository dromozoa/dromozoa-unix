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

extern "C" {
#include <lua.h>
}

#include <fcntl.h>

#include "error.hpp"
#include "fd.hpp"
#include "function.hpp"
#include "ndelay.hpp"
#include "success.hpp"

namespace dromozoa {
  int ndelay_on(int fd) {
    int result = fcntl(fd, F_GETFL);
    if (result == -1) {
      return -1;
    }
    return fcntl(fd, F_SETFL, result | O_NONBLOCK);
  }

  int ndelay_off(int fd) {
    int result = fcntl(fd, F_GETFL);
    if (result == -1) {
      return -1;
    }
    return fcntl(fd, F_SETFL, result & ~O_NONBLOCK);
  }

  namespace {
    int impl_ndealy_on(lua_State* L) {
      if (ndelay_on(get_fd(L, 1)) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_ndealy_off(lua_State* L) {
      if (ndelay_off(get_fd(L, 1)) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }
  }

  void initialize_ndelay(lua_State* L) {
    function<impl_ndealy_on>::set_field(L, "ndelay_on");
    function<impl_ndealy_off>::set_field(L, "ndelay_off");
  }
}
