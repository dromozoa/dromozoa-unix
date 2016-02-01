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
#include <lauxlib.h>
}

#include <errno.h>
#include <stddef.h>
#include <unistd.h>

#include <vector>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "fd.hpp"
#include "write.hpp"

namespace dromozoa {
  using bind::function;
  using bind::translate_range_i;
  using bind::translate_range_j;

  int impl_write(lua_State* L) {
    size_t size;
    const char* buffer = luaL_checklstring(L, 2, &size);
    size_t i = translate_range_i(L, 3, size);
    size_t j = translate_range_j(L, 4, size);
    if (i < j) {
      ssize_t result = write(get_fd(L, 1), buffer + i, j - i);
      if (result == -1) {
        int code = errno;
        if (code == EAGAIN || code == EWOULDBLOCK) {
          return push_resource_unavailable_try_again(L);
        } else {
          return push_error(L);
        }
      } else {
        lua_pushinteger(L, result);
        return 1;
      }
    } else {
      lua_pushinteger(L, 0);
      return 1;
    }
  }

  void initialize_write(lua_State* L) {
    function<impl_write>::set_field(L, "write");
  }
}
