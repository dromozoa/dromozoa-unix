// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <dromozoa/compat_pipe2.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_pipe(lua_State* L) {
      int flags = luaX_opt_integer<int>(L, 1, O_CLOEXEC);
      int fd[2] = { -1, -1 };
      if (compat_pipe2(fd, flags) == -1) {
        push_error(L);
      } else {
        new_fd(L, fd[0]);
        new_fd(L, fd[1]);
      }
    }
  }

  void initialize_pipe(lua_State* L) {
    luaX_set_field(L, -1, "pipe", impl_pipe);
  }
}
