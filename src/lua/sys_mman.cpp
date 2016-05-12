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

#include <alloca.h>
#include <string.h>
#include <sys/mman.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_mlockall(lua_State* L) {
      int flags = luaX_check_integer<int>(L, 1);
      if (mlockall(flags) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_munlockall(lua_State* L) {
      if (munlockall() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_reserve_stack_pages(lua_State* L) {
      size_t size = luaX_check_integer<size_t>(L, 1);
      void* buffer = alloca(size);
      memset(buffer, 0, size);
      luaX_push_success(L);
    }
  }

  void initialize_sys_mman(lua_State* L) {
    luaX_set_field(L, -1, "mlockall", impl_mlockall);
    luaX_set_field(L, -1, "munlockall", impl_munlockall);
    luaX_set_field(L, -1, "reserve_stack_pages", impl_reserve_stack_pages);

    luaX_set_field(L, -1, "MCL_CURRENT", MCL_CURRENT);
    luaX_set_field(L, -1, "MCL_FUTURE", MCL_FUTURE);
  }
}
