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
  namespace {
    void new_fd_ref(lua_State* L, int fd) {
      luaX_new<file_descriptor>(L, fd);
      luaX_set_metatable(L, "dromozoa.unix.fd_ref");
    }

    file_descriptor* check_file_descriptor(lua_State* L, int n) {
      return luaX_check_udata<file_descriptor>(L, n, "dromozoa.unix.fd_ref", "dromozoa.unix.fd");
    }

    void impl_gc(lua_State* L) {
      check_file_descriptor(L, 1)->~file_descriptor();
    }

    void impl_call(lua_State* L) {
      new_fd(L, check_fd(L, 2));
    }

    void impl_get(lua_State* L) {
      luaX_push(L, check_fd(L, 1));
    }

    void impl_close(lua_State* L) {
      int result = -1;
      if (lua_isuserdata(L, 1)) {
        result = check_file_descriptor(L, 1)->close();
      } else {
        result = file_descriptor(luaX_check_integer<int>(L, 1)).close();
      }
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_fd_ref(lua_State* L) {
      int fd = check_fd(L, 1);
      new_fd_ref(L, fd);
    }
  }

  void new_fd(lua_State* L, int fd) {
    luaX_new<file_descriptor>(L, fd);
    luaX_set_metatable(L, "dromozoa.unix.fd");
  }

  int to_fd(lua_State* L, int index) {
    if (lua_isuserdata(L, index)) {
      if (file_descriptor* self = luaX_to_udata<file_descriptor>(L, index, "dromozoa.unix.fd_ref", "dromozoa.unix.fd")) {
        return self->get();
      }
    } else if (lua_isnumber(L, index)) {
      return lua_tointeger(L, index);
    }
    return -1;
  }

  int check_fd(lua_State* L, int n) {
    if (lua_isuserdata(L, n)) {
      return check_file_descriptor(L, n)->get();
    } else {
      return luaX_check_integer<int>(L, n);
    }
  }

  void initialize_fd_djb(lua_State* L);
  void initialize_fd_socket(lua_State* L);
  void initialize_fd_unistd(lua_State* L);

  void initialize_fd(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.fd_ref");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      lua_pop(L, 1);

      luaL_newmetatable(L, "dromozoa.unix.fd");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, "__call", impl_call);
      luaX_set_field(L, -1, "get", impl_get);
      luaX_set_field(L, -1, "close", impl_close);

      initialize_fd_djb(L);
      initialize_fd_socket(L);
      initialize_fd_unistd(L);
    }
    luaX_set_field(L, -2, "fd");

    luaX_set_field(L, -1, "fd_ref", impl_fd_ref);
    new_fd_ref(L, 0);
    luaX_set_field(L, -2, "stdin");
    new_fd_ref(L, 1);
    luaX_set_field(L, -2, "stdout");
    new_fd_ref(L, 2);
    luaX_set_field(L, -2, "stderr");
  }
}
