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

#include <fcntl.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/lock.hpp>
#include <dromozoa/ndelay.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_coe(lua_State* L) {
      if (coe(check_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_ex(lua_State* L) {
      if (lock_ex(check_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_exnb(lua_State* L) {
      if (lock_exnb(check_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_un(lua_State* L) {
      if (lock_un(check_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_ndelay_on(lua_State* L) {
      if (ndelay_on(check_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_ndelay_off(lua_State* L) {
      if (ndelay_off(check_fd(L, 1)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_is_coe(lua_State* L) {
      int result = fcntl(check_fd(L, 1), F_GETFD);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, (result & FD_CLOEXEC) != 0);
      }
    }

    void impl_is_ndelay_on(lua_State* L) {
      int result = fcntl(check_fd(L, 1), F_GETFL);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, (result & O_NONBLOCK) != 0);
      }
    }

    void impl_is_ndelay_off(lua_State* L) {
      int result = fcntl(check_fd(L, 1), F_GETFL);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, (result & O_NONBLOCK) == 0);
      }
    }
  }

  void initialize_fd_djb(lua_State* L) {
    luaX_set_field(L, -1, "coe", impl_coe);
    luaX_set_field(L, -1, "lock_ex", impl_lock_ex);
    luaX_set_field(L, -1, "lock_exnb", impl_lock_exnb);
    luaX_set_field(L, -1, "lock_un", impl_lock_un);
    luaX_set_field(L, -1, "ndelay_on", impl_ndelay_on);
    luaX_set_field(L, -1, "ndelay_off", impl_ndelay_off);
    luaX_set_field(L, -1, "is_coe", impl_is_coe);
    luaX_set_field(L, -1, "is_ndelay_on", impl_is_ndelay_on);
    luaX_set_field(L, -1, "is_ndelay_off", impl_is_ndelay_off);
  }
}
