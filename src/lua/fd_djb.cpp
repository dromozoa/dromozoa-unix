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

#include <dromozoa/coe.hpp>
#include <dromozoa/lock.hpp>
#include <dromozoa/ndelay.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_coe(lua_State* L) {
      int fd = check_fd(L, 1);
      if (coe(fd) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_ex(lua_State* L) {
      int fd = check_fd(L, 1);
      if (lock_ex(fd) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_exnb(lua_State* L) {
      int fd = check_fd(L, 1);
      if (lock_exnb(fd) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_lock_un(lua_State* L) {
      int fd = check_fd(L, 1);
      if (lock_un(fd) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_ndelay_on(lua_State* L) {
      int fd = check_fd(L, 1);
      if (ndelay_on(fd) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_ndelay_off(lua_State* L) {
      int fd = check_fd(L, 1);
      if (ndelay_off(fd) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
  }

  void initialize_fd_djb(lua_State* L) {
    luaX_set_field(L, "coe", impl_coe);
    luaX_set_field(L, "lock_ex", impl_lock_ex);
    luaX_set_field(L, "lock_exnb", impl_lock_exnb);
    luaX_set_field(L, "lock_un", impl_lock_un);
    luaX_set_field(L, "ndelay_on", impl_ndelay_on);
    luaX_set_field(L, "ndelay_off", impl_ndelay_off);
  }
}
