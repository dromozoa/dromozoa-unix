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

#include "log_level.hpp"
#include "function.hpp"

namespace dromozoa {
  namespace {
    lua_Integer log_level = 0;
  }

  lua_Integer get_log_level() {
    return log_level;
  }

  namespace {
    int impl_set_log_level(lua_State* L) {
      log_level = luaL_checkinteger(L, 1);
      return 0;
    }

    int impl_get_log_level(lua_State* L) {
      lua_pushinteger(L, log_level);
      return 1;
    }
  }

  void initialize_log_level(lua_State* L) {
    function<impl_set_log_level>::set_field(L, "set_log_level");
    function<impl_get_log_level>::set_field(L, "get_log_level");
  }
}
