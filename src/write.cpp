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

#include <stddef.h>
#include <unistd.h>

#include <vector>

#include "error.hpp"
#include "fd.hpp"
#include "write.hpp"
#include "set_field.hpp"

namespace dromozoa {
  int impl_write(lua_State* L) {
    size_t size;
    const char* buffer = luaL_checklstring(L, 2, &size);
    ssize_t i = luaL_optinteger(L, 3, 1);
    if (i < 0) {
      i = size + i + 1;
    }
    if (i < 1) {
      i = 1;
    }
    ssize_t j = luaL_optinteger(L, 4, size);
    if (j < 0) {
      j = size + j + 1;
    }
    if (j > static_cast<ssize_t>(size)) {
      j = size;
    }
    if (i <= j) {
      const char* p = buffer + i - 1;
      size_t n = j - (i - 1);
      ssize_t result = write(get_fd(L, 1), p, n);
      if (result == -1) {
        return push_error(L);
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
    set_field(L, "write", impl_write);
  }
}
