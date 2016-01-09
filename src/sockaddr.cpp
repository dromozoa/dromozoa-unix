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

#include <string.h>

#include "function.hpp"
#include "sockaddr.hpp"

namespace dromozoa {
  int new_sockaddr(lua_State* L, const void* address, socklen_t size) {
    memcpy(lua_newuserdata(L, size), address, size);
    luaL_getmetatable(L, "dromozoa.unix.sockaddr");
    lua_setmetatable(L, -2);
    return 1;
  }

  namespace {
    sockaddr* get_sockaddr(lua_State* L, int n) {
      return static_cast<sockaddr*>(luaL_checkudata(L, n, "dromozoa.unix.sockaddr"));
    }

    int impl_family(lua_State* L) {
      lua_pushinteger(L, get_sockaddr(L, 1)->sa_family);
      return 1;
    }
  }

  int open_sockaddr(lua_State* L) {
    lua_newtable(L);
    function<impl_family>::set_field(L, "family");
    luaL_newmetatable(L, "dromozoa.unix.sockaddr");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
    return 1;
  }
}
