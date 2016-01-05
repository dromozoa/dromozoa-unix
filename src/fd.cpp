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

#include "error.hpp"
#include "fd.hpp"
#include "log_level.hpp"
#include "set_field.hpp"

namespace dromozoa {
  void new_fd(lua_State* L, int fd) {
    *static_cast<int*>(lua_newuserdata(L, sizeof(int))) = fd;
    luaL_getmetatable(L, "dromozoa.unix.fd");
    lua_setmetatable(L, -2);
  }

  void set_fd(lua_State* L, int n, int fd) {
    if (int* data = static_cast<int*>(lua_touserdata(L, n))) {
      *data = fd;
    } else {
      luaL_argerror(L, n, "userdata expected, got nil");
    }
  }

  int get_fd(lua_State* L, int n) {
    if (const int* data = static_cast<const int*>(lua_touserdata(L, n))) {
      return *data;
    } else {
      luaL_argerror(L, n, "userdata expected, got nil");
      return -1;
    }
  }

  namespace {
    int impl_close(lua_State* L) {
      int fd = get_fd(L, 1);
      set_fd(L, 1, -1);
      if (close(fd) == -1) {
        return push_error(L);
      } else {
        lua_pushinteger(L, 0);
        return 1;
      }
    }

    int impl_gc(lua_State* L) {
      int fd = get_fd(L, 1);
      if (fd != -1) {
        if (get_log_level() > 1) {
          std::cerr << "[dromozoa-unix] fd " << fd << " detected" << std::endl;
        }
        set_fd(L, 1, -1);
        if (close(fd) == -1) {
          if (get_log_level() > 0) {
            std::cerr << "[dromozoa-unix] cannot close fd " << fd << ": ";
            print_error(std::cerr);
            std::cerr << std::endl;
          }
        }
      }
      return 0;
    }
  }

  void initialize_fd(lua_State* L) {
    lua_newtable(L);
    set_field(L, "close", impl_close);
    luaL_newmetatable(L, "dromozoa.unix.fd");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    set_field(L, "__gc", impl_gc);
    lua_pop(L, 1);
  }
}
