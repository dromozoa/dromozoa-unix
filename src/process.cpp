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

#include "dromozoa/bind.hpp"

#include "argument_vector.hpp"
#include "error.hpp"
#include "fd.hpp"
#include "forkexec.hpp"

namespace dromozoa {
  using bind::function;
  using bind::push_success;

  namespace {
    int impl_new(lua_State* L) {
      lua_newtable(L);
      luaL_getmetatable(L, "dromozoa.unix.process");
      lua_setmetatable(L, -2);
      return 1;
    }

    int impl_forkexec(lua_State* L) {
      const char* path = luaL_checkstring(L, 2);
      luaL_checktype(L, 3, LUA_TTABLE);
      argument_vector argv(L, 3);
      argument_vector envp(L, 4);
      const char* chdir = lua_tostring(L, 5);
      int dup2_stdio[3] = { -1, -1, -1 };
      if (lua_istable(L, 6)) {
        for (int i = 0; i < 3; ++i) {
          lua_pushinteger(L, i);
          lua_gettable(L, 6);
          if (!lua_isnil(L, -1)) {
            dup2_stdio[i] = get_fd(L, -1);
          }
          lua_pop(L, 1);
        }
      }
      pid_t pid = -1;
      int result = forkexec(path, argv.get(), envp.get(), chdir, dup2_stdio, pid);
      int code = errno;
      if (pid != -1) {
        lua_pushinteger(L, 1);
        lua_pushinteger(L, pid);
        lua_settable(L, 1);
      }
      if (result == -1) {
        return push_error(L, code);
      } else {
        return push_success(L);
      }
    }

    int impl_forkexec_daemon(lua_State* L) {
      const char* path = luaL_checkstring(L, 2);
      luaL_checktype(L, 3, LUA_TTABLE);
      argument_vector argv(L, 3);
      argument_vector envp(L, 4);
      const char* chdir = lua_tostring(L, 5);
      pid_t pid1 = -1;
      pid_t pid2 = -1;
      int result = forkexec_daemon(path, argv.get(), envp.get(), chdir, pid1, pid2);
      int code = errno;
      if (pid1 != -1) {
        lua_pushinteger(L, 1);
        lua_pushinteger(L, pid1);
        lua_settable(L, 1);
      }
      if (pid2 != -1) {
        lua_pushinteger(L, 2);
        lua_pushinteger(L, pid2);
        lua_settable(L, 1);
      }
      if (result == -1) {
        return push_error(L, code);
      } else {
        return push_success(L);
      }
    }
  }

  int open_process(lua_State* L) {
    lua_newtable(L);
    function<impl_forkexec>::set_field(L, "forkexec");
    function<impl_forkexec_daemon>::set_field(L, "forkexec_daemon");
    lua_newtable(L);
    function<impl_new>::set_field(L, "__call");
    lua_setmetatable(L, -2);
    luaL_newmetatable(L, "dromozoa.unix.process");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
    return 1;
  }
}
