// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
  argument_vector to_argument_vector(lua_State* L, int index) {
    argument_vector result;
    if (lua_istable(L, index)) {
      result.clear(); // make empty
      for (int i = 1; ; ++i) {
        luaX_get_field(L, index, i);
        if (const char* p = lua_tostring(L, -1)) {
          result.push_back(p);
          lua_pop(L, 1);
        } else {
          lua_pop(L, 1);
          break;
        }
      }
    }
    return result;
  }
}
