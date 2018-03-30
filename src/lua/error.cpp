// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/errno_saver.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void set_last_errno(lua_State* L, int code) {
      luaX_set_field(L, LUA_REGISTRYINDEX, "dromozoa.unix.last_errno", code);
    }

    int get_last_errno(lua_State* L) {
      return luaX_opt_integer_field<int>(L, LUA_REGISTRYINDEX, "dromozoa.unix.last_errno", 0);
    }

    void impl_strerror(lua_State* L) {
      errno_saver save_errno;
      std::string message = compat_strerror(luaX_opt_integer<int>(L, 1, save_errno.get()));
      luaX_push(L, message);
    }

    void impl_set_errno(lua_State* L) {
      errno = luaX_check_integer<int>(L, 1);
      luaX_push_success(L);
    }

    void impl_get_errno(lua_State* L) {
      luaX_push(L, errno);
    }

    void impl_get_error(lua_State* L) {
      errno_saver save_errno;
      std::string message = compat_strerror(save_errno.get());
      luaX_push(L, luaX_nil);
      luaX_push(L, message);
      luaX_push(L, save_errno.get());
    }

    void impl_set_last_errno(lua_State* L) {
      set_last_errno(L, luaX_check_integer<int>(L, 1));
      luaX_push_success(L);
    }

    void impl_get_last_errno(lua_State* L) {
      luaX_push(L, get_last_errno(L));
    }

    void impl_get_last_error(lua_State* L) {
      int code = get_last_errno(L);
      std::string message = compat_strerror(code);
      luaX_push(L, luaX_nil);
      luaX_push(L, message);
      luaX_push(L, code);
    }
  }

  void push_error(lua_State* L) {
    errno_saver save_errno;
    set_last_errno(L, save_errno.get());
    std::string message = compat_strerror(save_errno.get());
    luaX_push(L, luaX_nil);
    luaX_push(L, message);
    luaX_push(L, save_errno.get());
  }

  void initialize_error(lua_State* L) {
    luaX_set_field(L, -1, "strerror", impl_strerror);
    luaX_set_field(L, -1, "set_errno", impl_set_errno);
    luaX_set_field(L, -1, "get_errno", impl_get_errno);
    luaX_set_field(L, -1, "get_error", impl_get_error);
    luaX_set_field(L, -1, "set_last_errno", impl_set_last_errno);
    luaX_set_field(L, -1, "get_last_errno", impl_get_last_errno);
    luaX_set_field(L, -1, "get_last_error", impl_get_last_error);
  }
}
