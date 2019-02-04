// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <sys/utsname.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_uname(lua_State* L) {
      struct utsname name = {};
      if (uname(&name) == -1) {
        push_error(L);
      } else {
        lua_newtable(L);
        luaX_set_field(L, -1, "sysname", name.sysname);
        luaX_set_field(L, -1, "nodename", name.nodename);
        luaX_set_field(L, -1, "release", name.release);
        luaX_set_field(L, -1, "version", name.version);
        luaX_set_field(L, -1, "machine", name.machine);
      }
    }
  }

  void initialize_sys_utsname(lua_State* L) {
    luaX_set_field(L, -1, "uname", impl_uname);
  }
}
