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
#include "read.hpp"

namespace dromozoa {
  using bind::function;

  namespace {
    int impl_read(lua_State* L) {
      std::vector<char> buffer(luaL_checkinteger(L, 2));
      ssize_t result = read(get_fd(L, 1), &buffer[0], buffer.size());
      if (result == -1) {
        return push_error(L);
      } else {
        lua_pushlstring(L, &buffer[0], result);
        return 1;
      }
    }
  }

  void initialize_read(lua_State* L) {
    function<impl_read>::set_field(L, "read");
  }
}
