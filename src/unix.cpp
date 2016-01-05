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

#include "error.hpp"
#include "fd.hpp"
#include "log_level.hpp"
#include "pipe2.hpp"
#include "set_field.hpp"

namespace dromozoa {
  int unix_fd_gc(lua_State* L) {
    int fd = get_fd(L, 1);
    if (fd != -1) {
      set_fd(L, 1, -1);
      close(fd);
    }
    return 0;
  }

  int unix_fd_close(lua_State* L) {
    int fd = get_fd(L, 1);
    if (fd != -1) {
      set_fd(L, 1, -1);
      if (close(fd) == -1) {
        return push_error(L);
      }
    }
    lua_pushinteger(L, 0);
    return 1;
  }

  int unix_pipe2(lua_State* L) {
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
  dromozoa::initialize_fd(L);
  lua_setfield(L, -2, "fd");
  dromozoa::set_field(L, "pipe2", dromozoa::unix_pipe2);
  dromozoa::initialize_log_level(L);
  return 1;
}
