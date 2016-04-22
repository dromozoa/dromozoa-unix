// Copyright (C) 2015,2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_nanosleep(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      check_timespec(L, 1, tv1);
      if (nanosleep(&tv1, &tv2) != -1) {
        luaX_push_success(L);
      } else {
        push_error(L);
        new_timespec(L, tv2);
      }
    }
  }

  void initialize_time(lua_State* L) {
    luaX_set_field(L, -1, "nanosleep", impl_nanosleep);
  }
}
