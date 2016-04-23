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

#include <fcntl.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_open(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      int flags = luaX_opt_integer<int>(L, 2, O_RDONLY | O_CLOEXEC);
      int mode = luaX_opt_integer<int>(L, 3, 0666);
      int result = open(path, flags, mode);
      if (result == -1) {
        push_error(L);
      } else {
        new_fd(L, result);
      }
    }
  }

  void initialize_fcntl(lua_State* L) {
    luaX_set_field(L, -1, "open", impl_open);

    luaX_set_field(L, -1, "O_APPEND", O_APPEND);
    luaX_set_field(L, -1, "O_CLOEXEC", O_CLOEXEC);
    luaX_set_field(L, -1, "O_CREAT", O_CREAT);
    luaX_set_field(L, -1, "O_NONBLOCK", O_NONBLOCK);
    luaX_set_field(L, -1, "O_RDONLY", O_RDONLY);
    luaX_set_field(L, -1, "O_RDWR", O_RDWR);
    luaX_set_field(L, -1, "O_SYNC", O_SYNC);
    luaX_set_field(L, -1, "O_WRONLY", O_WRONLY);
  }
}
