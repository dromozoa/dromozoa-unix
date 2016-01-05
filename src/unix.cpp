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
}

#include "coe.hpp"
#include "fcntl.hpp"
#include "fd.hpp"
#include "log_level.hpp"
#include "nanosleep.hpp"
#include "ndelay.hpp"
#include "pipe.hpp"

namespace dromozoa {
  int open(lua_State* L) {
    lua_newtable(L);

    open_fd(L);
    initialize_coe(L);
    initialize_ndelay(L);
    lua_setfield(L, -2, "fd");

    open_fcntl(L);
    lua_setfield(L, -2, "fcntl");

    dromozoa::initialize_log_level(L);
    dromozoa::initialize_nanosleep(L);
    dromozoa::initialize_pipe(L);

    return 1;
  }
}

extern "C" int luaopen_dromozoa_unix(lua_State* L) {
  return dromozoa::open(L);
}
