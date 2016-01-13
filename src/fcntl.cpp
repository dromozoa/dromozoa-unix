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

#include <fcntl.h>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "fcntl.hpp"
#include "fd.hpp"

namespace dromozoa {
  using bind::function;

  namespace {
    int impl_open(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      int flags = luaL_optinteger(L, 2, 0);
      int mode = luaL_optinteger(L, 3, 0);
      int result = open(path, flags, mode);
      if (result == -1) {
        return push_error(L);
      } else {
        new_fd(L, result);
        return 1;
      }
    }
  }

  void initialize_fcntl(lua_State* L) {
    function<impl_open>::set_field(L, "open");
    DROMOZOA_BIND_SET_FIELD(L, O_APPEND);
    DROMOZOA_BIND_SET_FIELD(L, O_CLOEXEC);
    DROMOZOA_BIND_SET_FIELD(L, O_CREAT);
    DROMOZOA_BIND_SET_FIELD(L, O_NONBLOCK);
    DROMOZOA_BIND_SET_FIELD(L, O_RDONLY);
    DROMOZOA_BIND_SET_FIELD(L, O_RDWR);
    DROMOZOA_BIND_SET_FIELD(L, O_WRONLY);
  }
}
