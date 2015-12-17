// Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <sys/time.h>
#include "common.hpp"

namespace dromozoa {
  int unix_gettimeofday(lua_State* L) {
    struct timeval tv = {};
    if (gettimeofday(&tv, 0) != -1) {
      lua_newtable(L);
      lua_pushinteger(L, tv.tv_sec);
      lua_setfield(L, -2, "tv_sec");
      lua_pushinteger(L, tv.tv_usec);
      lua_setfield(L, -2, "tv_usec");
      return 1;
    } else {
      return push_error(L);
    }
  }
}

extern "C" int luaopen_dromozoa_unix_gettimeofday(lua_State* L) {
  lua_pushcfunction(L, dromozoa::unix_gettimeofday);
  return 1;
}
