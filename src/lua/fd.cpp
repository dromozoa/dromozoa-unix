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

#include <new>

#include <dromozoa/file_descriptor.hpp>

#include "common.hpp"

namespace dromozoa {
  int new_fd(lua_State* L, int fd, bool ref) {
    new(new_userdata<file_descriptor>(L)) file_descriptor(fd);
    if (ref) {
      set_metatable(L, "dromozoa.unix.fd.ref");
    } else {
      set_metatable(L, "dromozoa.unix.fd");
    }
    return 1;
  }

  namespace {
    file_descriptor* get_file_descriptor(lua_State* L, int n) {
      return check_userdata<file_descriptor>(L, n, "dromozoa.unix.fd.ref", "dromozoa.unix.fd");
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

  int open_fd(lua_State* L) {
    lua_newtable(L);
    set_field(L, "get", function<impl_get>());
    set_field(L, "close", function<impl_close>());
    set_metafield(L, "__call", function<impl_new>());

    new_metatable(L, "dromozoa.unix.fd.ref");
    new_metatable(L, "dromozoa.unix.fd", function<impl_gc>());

    new_fd(L, 0, true);
    lua_setfield(L, -2, "stdin");
    new_fd(L, 1, true);
    lua_setfield(L, -2, "stdout");
    new_fd(L, 2, true);
    lua_setfield(L, -2, "stderr");
    return 1;
  }
}
