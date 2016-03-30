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
#include <stddef.h>
#include <unistd.h>

#include <vector>

#include "common.hpp"

namespace dromozoa {
  void impl_write(lua_State* L) {
    size_t size;
    const char* buffer = luaL_checklstring(L, 2, &size);
    size_t i = luaX_range_i(L, 3, size);
    size_t j = luaX_range_j(L, 4, size);
    if (i < j) {
      ssize_t result = write(get_fd(L, 1), buffer + i, j - i);
      if (result == -1) {
        int code = errno;
        if (code == EAGAIN || code == EWOULDBLOCK) {
          push_resource_unavailable_try_again(L);
        } else if (code == EPIPE) {
          push_broken_pipe(L);
        } else {
          push_error(L);
        }
      } else {
        lua_pushinteger(L, result);
      }
    } else {
      lua_pushinteger(L, 0);
    }
  }

  void initialize_write(lua_State* L) {
    luaX_set_field(L, "write", impl_write);
  }
}
