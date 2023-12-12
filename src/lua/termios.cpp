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
    void impl_get_iflag(lua_State* L) {
      luaX_push(L, check_termios(L, 1)->c_iflag);
    }

    void impl_get_oflag(lua_State* L) {
      luaX_push(L, check_termios(L, 1)->c_oflag);
    }

    void impl_get_cflag(lua_State* L) {
      luaX_push(L, check_termios(L, 1)->c_cflag);
    }

    void impl_get_lflag(lua_State* L) {
      luaX_push(L, check_termios(L, 1)->c_lflag);
    }

    void impl_get_cc(lua_State* L) {
      const struct termios* self = check_termios(L, 1);
      size_t i = luaX_check_integer<size_t>(L, 2, 0, NCCS - 1);
      luaX_push(L, self->c_cc[i]);
    }

    void impl_cfgetispeed(lua_State* L) {
      luaX_push(L, cfgetispeed(check_termios(L, 1)));
    }

    void impl_cfgetospeed(lua_State* L) {
      luaX_push(L, cfgetospeed(check_termios(L, 1)));
    }

    void impl_cfsetispeed(lua_State* L) {
      struct termios* self = check_termios(L, 1);
      speed_t speed = luaX_check_integer<speed_t>(L, 2);
      if (cfsetispeed(self, speed) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_cfsetospeed(lua_State* L) {
      struct termios* self = check_termios(L, 1);
      speed_t speed = luaX_check_integer<speed_t>(L, 2);
      if (cfsetospeed(self, speed) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
  }

  void new_termios(lua_State* L, const struct termios& termios) {
    luaX_new<struct termios>(L, termios);
    luaX_set_metatable(L, "dromozoa.unix.termios");
  }

  struct termios* check_termios(lua_State* L, int arg) {
    return luaX_check_udata<struct termios>(L, arg, "dromozoa.unix.termios");
  }

  void initialize_termios(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.termios");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      lua_pop(L, 1);

      luaX_set_field(L, -1, "get_iflag", impl_get_iflag);
      luaX_set_field(L, -1, "get_oflag", impl_get_oflag);
      luaX_set_field(L, -1, "get_cflag", impl_get_cflag);
      luaX_set_field(L, -1, "get_lflag", impl_get_lflag);
      luaX_set_field(L, -1, "get_cc", impl_get_cc);

      luaX_set_field(L, -1, "cfgetispeed", impl_cfgetispeed);
      luaX_set_field(L, -1, "cfgetospeed", impl_cfgetospeed);

      luaX_set_field(L, -1, "cfsetispeed", impl_cfsetispeed);
      luaX_set_field(L, -1, "cfsetospeed", impl_cfsetospeed);

      luaX_set_field(L, -1, "NCCS", NCCS);

      luaX_set_field(L, -1, "B0", B0);
      luaX_set_field(L, -1, "B50", B50);
      luaX_set_field(L, -1, "B75", B75);
      luaX_set_field(L, -1, "B110", B110);
      luaX_set_field(L, -1, "B134", B134);
      luaX_set_field(L, -1, "B150", B150);
      luaX_set_field(L, -1, "B200", B200);
      luaX_set_field(L, -1, "B300", B300);
      luaX_set_field(L, -1, "B600", B600);
      luaX_set_field(L, -1, "B1200", B1200);
      luaX_set_field(L, -1, "B1800", B1800);
      luaX_set_field(L, -1, "B2400", B2400);
      luaX_set_field(L, -1, "B4800", B4800);
      luaX_set_field(L, -1, "B9600", B9600);
      luaX_set_field(L, -1, "B19200", B19200);
      luaX_set_field(L, -1, "B38400", B38400);

      luaX_set_field(L, -1, "CSIZE", CSIZE);
      luaX_set_field(L, -1, "CS5", CS5);
      luaX_set_field(L, -1, "CS6", CS6);
      luaX_set_field(L, -1, "CS7", CS7);
      luaX_set_field(L, -1, "CS8", CS8);

      luaX_set_field(L, -1, "CSTOPB", CSTOPB);
      luaX_set_field(L, -1, "CREAD", CREAD);
      luaX_set_field(L, -1, "PARENB", PARENB);
      luaX_set_field(L, -1, "PARODD", PARODD);
      luaX_set_field(L, -1, "HUPCL", HUPCL);
      luaX_set_field(L, -1, "CLOCAL", CLOCAL);

      luaX_set_field(L, -1, "ECHO", ECHO);
      luaX_set_field(L, -1, "ECHOE", ECHOE);
      luaX_set_field(L, -1, "ECHOK", ECHOK);
      luaX_set_field(L, -1, "ECHONL", ECHONL);
      luaX_set_field(L, -1, "ICANON", ICANON);
      luaX_set_field(L, -1, "IEXTEN", IEXTEN);
      luaX_set_field(L, -1, "ISIG", ISIG);
      luaX_set_field(L, -1, "NOFLSH", NOFLSH);
      luaX_set_field(L, -1, "TOSTOP", TOSTOP);

      luaX_set_field(L, -1, "TCSANOW", TCSANOW);
      luaX_set_field(L, -1, "TCSADRAIN", TCSADRAIN);
      luaX_set_field(L, -1, "TCSAFLUSH", TCSAFLUSH);
    }
    luaX_set_field(L, -2, "termios");
  }
}
