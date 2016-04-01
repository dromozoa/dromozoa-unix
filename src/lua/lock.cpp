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

#include <errno.h>

#include <dromozoa/lock.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_lock_ex(lua_State* L) {
      if (lock_ex(get_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_exnb(lua_State* L) {
      if (lock_exnb(get_fd(L, 1)) == -1) {
        int code = errno;
        if (code == EWOULDBLOCK) {
          push_resource_unavailable_try_again(L);
        } else {
          push_error(L);
        }
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_un(lua_State* L) {
      if (lock_un(get_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
  }

  void initialize_lock(lua_State* L) {
    luaX_set_field(L, "lock_ex", impl_lock_ex);
    luaX_set_field(L, "lock_exnb", impl_lock_exnb);
    luaX_set_field(L, "lock_un", impl_lock_un);
  }
}
