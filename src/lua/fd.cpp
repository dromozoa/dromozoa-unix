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

#include <unistd.h>

#include <iostream>

#include <dromozoa/bind.hpp>
#include <dromozoa/file_descriptor.hpp>

#include "error.hpp"
#include "fd.hpp"

namespace dromozoa {
  using bind::function;
  using bind::get_log_level;
  using bind::push_success;

  int new_fd(lua_State* L, int fd, bool ref) {
    file_descriptor* data = static_cast<file_descriptor*>(lua_newuserdata(L, sizeof(file_descriptor)));
    new(data) file_descriptor(fd);
    if (ref) {
      luaL_getmetatable(L, "dromozoa.unix.fd.ref");
    } else {
      luaL_getmetatable(L, "dromozoa.unix.fd");
    }
    lua_setmetatable(L, -2);
    if (get_log_level() > 2) {
      if (ref) {
        std::cerr << "[dromozoa-unix] new fd-ref " << fd << std::endl;
      } else {
        std::cerr << "[dromozoa-unix] new fd " << fd << std::endl;
      }
    }
    return 1;
  }

  namespace {
    file_descriptor* get_file_descriptor(lua_State* L, int n) {
      if (file_descriptor* data = static_cast<file_descriptor*>(luaL_testudata(L, n, "dromozoa.unix.fd.ref"))) {
        return data;
      } else {
        return static_cast<file_descriptor*>(luaL_checkudata(L, n, "dromozoa.unix.fd"));
      }
    }
  }

  int get_fd(lua_State* L, int n) {
    if (lua_isuserdata(L, n)) {
      return get_file_descriptor(L, n)->get();
    } else {
      return luaL_checkinteger(L, n);
    }
  }

  namespace {
    int impl_new(lua_State* L) {
      int fd = get_fd(L, 2);
      bool ref = lua_toboolean(L, 3);
      return new_fd(L, fd, ref);
    }

    int impl_get(lua_State* L) {
      lua_pushinteger(L, get_fd(L, 1));
      return 1;
    }

    int impl_close(lua_State* L) {
      int result = -1;
      if (lua_isuserdata(L, 1)) {
        result = get_file_descriptor(L, 1)->close();
      } else {
        result = file_descriptor(luaL_checkinteger(L, 1)).close();
      }
      if (result == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_gc(lua_State* L) {
      get_file_descriptor(L, 1)->~file_descriptor();
      return 0;
    }
  }

  /*
    local class = {}
    function class:get() end;
    function class:close() end;

    local metatable = {}
    function metatable:__call()

    setmetatable(class, metatable)

    local dromozoa_unix_fd = {}
    dromozoa_unix_fd.__index = class
    function dromozoa_unix_fd:__gc()

    local dromozoa_unix_fd_ref = {}
    dromozoa_unix_fd_ref.__index = class

   */
  int open_fd(lua_State* L) {
    lua_newtable(L);
    function<impl_get>::set_field(L, "get");
    function<impl_close>::set_field(L, "close");
    lua_newtable(L);
    function<impl_new>::set_field(L, "__call");
    lua_setmetatable(L, -2);

    luaL_newmetatable(L, "dromozoa.unix.fd.ref");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    luaL_newmetatable(L, "dromozoa.unix.fd");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);

    new_fd(L, 0, true);
    lua_setfield(L, -2, "stdin");
    new_fd(L, 1, true);
    lua_setfield(L, -2, "stdout");
    new_fd(L, 2, true);
    lua_setfield(L, -2, "stderr");
    return 1;
  }
}
