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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dromozoa/timer.hpp>

#if defined(HAVE_CLOCK_GETTIME)
#include <dromozoa/timer_clock_gettime.hpp>
typedef dromozoa::timer_clock_gettime timer_impl;
#elif defined(HAVE_MACH_ABSOLUTE_TIME)
#include <dromozoa/timer_mach_absolute_time.hpp>
typedef dromozoa::timer_mach_absolute_time timer_impl;
#endif

#include "common.hpp"

namespace dromozoa {
  namespace {
    timer* check_timer(lua_State* L, int arg) {
      return luaX_check_udata<timer>(L, arg, "dromozoa.unix.timer");
    }

    void impl_gc(lua_State* L) {
      check_timer(L, 1)->~timer();
    }

    void impl_call(lua_State* L) {
      luaX_new<timer_impl>(L);
      luaX_set_metatable(L, "dromozoa.unix.timer");
    }

    void impl_start(lua_State* L) {
      if (check_timer(L, 1)->start() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_stop(lua_State* L) {
      if (check_timer(L, 1)->stop() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_elapsed(lua_State* L) {
      luaX_push(L, check_timer(L, 1)->elapsed());
    }
  }

  void initialize_timer(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.timer");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "start", impl_start);
      luaX_set_field(L, -1, "stop", impl_stop);
      luaX_set_field(L, -1, "elapsed", impl_elapsed);
    }
    luaX_set_field(L, -2, "timer");
  }
}
