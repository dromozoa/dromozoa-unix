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

#include <dromozoa/file_descriptor.hpp>

#include "common.hpp"

namespace dromozoa {
  void new_fd(lua_State* L, int fd) {
    luaX_new<file_descriptor>(L, fd);
    luaX_set_metatable(L, "dromozoa.unix.fd");
  }

  namespace {
    void new_fd_ref(lua_State* L, int fd) {
      luaX_new<file_descriptor>(L, fd);
      luaX_set_metatable(L, "dromozoa.unix.fd.ref");
    }

    file_descriptor* get_file_descriptor(lua_State* L, int index) {
      return luaX_check_udata<file_descriptor>(L, index, "dromozoa.unix.fd.ref", "dromozoa.unix.fd");
    }
  }

  int get_fd(lua_State* L, int index) {
    if (lua_isuserdata(L, index)) {
      return get_file_descriptor(L, index)->get();
    } else {
      return luaL_checkinteger(L, index);
    }
  }

  namespace {
    void impl_call(lua_State* L) {
      new_fd_ref(L, get_fd(L, 2));
    }

    void impl_get(lua_State* L) {
      lua_pushinteger(L, get_fd(L, 1));
    }

    void impl_close(lua_State* L) {
      int result = -1;
      if (lua_isuserdata(L, 1)) {
        result = get_file_descriptor(L, 1)->close();
      } else {
        result = file_descriptor(luaL_checkinteger(L, 1)).close();
      }
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_gc(lua_State* L) {
      get_file_descriptor(L, 1)->~file_descriptor();
    }
  }

  int open_fd(lua_State* L) {
    lua_newtable(L);

    luaL_newmetatable(L, "dromozoa.unix.fd.ref");
    lua_pushvalue(L, -2);
    luaX_set_field(L, "__index");
    lua_pop(L, 1);

    luaL_newmetatable(L, "dromozoa.unix.fd");
    lua_pushvalue(L, -2);
    luaX_set_field(L, "__index");
    luaX_set_field(L, "__gc", impl_gc);
    lua_pop(L, 1);

    luaX_set_metafield(L, "__call", impl_call);
    luaX_set_field(L, "get", impl_get);
    luaX_set_field(L, "close", impl_close);

    new_fd_ref(L, 0);
    luaX_set_field(L, "stdin");
    new_fd_ref(L, 1);
    luaX_set_field(L, "stdout");
    new_fd_ref(L, 2);
    luaX_set_field(L, "stderr");
    return 1;
  }
}
