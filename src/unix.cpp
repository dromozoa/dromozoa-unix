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

#include "coe.hpp"
#include "error.hpp"
#include "fd.hpp"
#include "log_level.hpp"
#include "pipe2.hpp"
#include "set_field.hpp"

namespace dromozoa {
  int impl_pipe2(lua_State* L) {
    int fd[2] = { -1, -1 };
    int flags = luaL_optinteger(L, 1, 0);
    if (pipe2(fd, flags) == -1) {
      return push_error(L);
    } else {
      new_fd(L, fd[0]);
      new_fd(L, fd[1]);
      return 2;
    }
  }
}

extern "C" int luaopen_dromozoa_unix(lua_State* L) {
  lua_newtable(L);
  dromozoa::open_fd(L);
  dromozoa::initialize_coe(L);
  lua_setfield(L, -2, "fd");
  dromozoa::set_field(L, "pipe2", dromozoa::impl_pipe2);
  dromozoa::initialize_log_level(L);
  return 1;
}
