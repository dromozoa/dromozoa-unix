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

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_band(lua_State* L) {
      int result = luaX_check_integer<int>(L, 1);
      int top = lua_gettop(L);
      for (int i = 2; i <= top; ++i) {
        result &= luaX_check_integer<int>(L, i);
      }
      luaX_push(L, result);
    }

    void impl_bor(lua_State* L) {
      int result = luaX_check_integer<int>(L, 1);
      int top = lua_gettop(L);
      for (int i = 2; i <= top; ++i) {
        result |= luaX_check_integer<int>(L, i);
      }
      luaX_push(L, result);
    }
  }

  void initialize_int(lua_State* L) {
    luaX_set_field(L, -1, "band", impl_band);
    luaX_set_field(L, -1, "bor", impl_bor);
  }
}
