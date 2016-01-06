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

#include "coe.hpp"
#include "error.hpp"
#include "fd.hpp"
#include "function.hpp"
#include "success.hpp"

namespace dromozoa {
  int coe(int fd) {
    int result = fcntl(fd, F_GETFD);
    if (result == -1) {
      return -1;
    }
    return fcntl(fd, F_SETFD, result | FD_CLOEXEC);
  }

  namespace {
    int impl_coe(lua_State* L) {
      if (coe(get_fd(L, 1)) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }
  }

  void initialize_coe(lua_State* L) {
    function<impl_coe>::set_field(L, "coe");
  }
}
