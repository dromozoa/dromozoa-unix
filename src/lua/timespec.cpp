// Copyright (C) 2015,2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <math.h>

#include "common.hpp"

namespace dromozoa {
  bool check_timespec(lua_State* L, int n, struct timespec& tv) {
    if (lua_isnoneornil(L, n)) {
      return false;
    } else if (lua_isnumber(L, n)) {
      double t = lua_tonumber(L, n);
      double i = 0;
      double f = modf(t, &i);
      tv.tv_sec = i;
      tv.tv_nsec = f * 1000000000;
      return true;
    } else if (lua_istable(L, n)) {
      tv.tv_sec = luaX_opt_integer_field<time_t>(L, n, "tv_sec", 0);
      tv.tv_nsec = luaX_opt_integer_field<long>(L, n, "tv_nsec", 0, 0L, 999999999L);
      return true;
    } else {
      luaL_argerror(L, n, "nil, number or table expected");
      return false;
    }
  }
}
