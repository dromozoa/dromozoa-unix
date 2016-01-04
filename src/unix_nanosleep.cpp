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
#include <lauxlib.h>
}

#include <time.h>
#include "common.hpp"

namespace dromozoa {
  int unix_nanosleep(lua_State* L) {
    struct timespec tv1 = {};
    struct timespec tv2 = {};

    lua_getfield(L, 1, "tv_sec");
    tv1.tv_sec = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 1, "tv_nsec");
    tv1.tv_nsec = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    if (nanosleep(&tv1, &tv2) != -1) {
      lua_pushinteger(L, 0);
      return 1;
    } else {
      int result = push_error(L);
      lua_newtable(L);
      lua_pushinteger(L, tv2.tv_sec);
      lua_setfield(L, -2, "tv_sec");
      lua_pushinteger(L, tv2.tv_nsec);
      lua_setfield(L, -2, "tv_nsec");
      return result + 1;
    }
  }
}

extern "C" int luaopen_dromozoa_unix_nanosleep(lua_State* L) {
  lua_pushcfunction(L, dromozoa::unix_nanosleep);
  return 1;
}
