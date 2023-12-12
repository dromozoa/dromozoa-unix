// Copyright (C) 2023 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <termios.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_tcgetattr(lua_State* L) {
      struct termios termios = {};
      if (tcgetattr(check_fd(L, 1), &termios) == -1) {
        push_error(L);
      } else {
        new_termios(L, termios);
      }
    }

    void impl_tcsetattr(lua_State* L) {
      int actions = luaX_check_integer<int>(L, 2);
      const struct termios* termios = check_termios(L, 3);
      if (tcsetattr(check_fd(L, 1), actions, termios) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
  }

  void initialize_fd_termios(lua_State* L) {
    luaX_set_field(L, -1, "tcgetattr", impl_tcgetattr);
    luaX_set_field(L, -1, "tcsetattr", impl_tcsetattr);
  }
}
