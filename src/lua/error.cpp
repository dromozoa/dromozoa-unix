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

#include <errno.h>

#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/errno_saver.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_strerror(lua_State* L) {
      errno_saver save;
      std::string message = compat_strerror(luaX_opt_integer<int>(L, 1, errno));
      lua_pushlstring(L, message.c_str(), message.size());
    }

    void impl_set_errno(lua_State* L) {
      errno = luaX_check_integer<int>(L, 1);
      luaX_push_success(L);
    }

    void impl_get_errno(lua_State* L) {
      luaX_push(L, errno);
    }
  }

  void push_error(lua_State* L) {
    push_error(L, errno);
  }

  void push_error(lua_State* L, int code) {
    errno_saver save;
    std::string message = compat_strerror(code);
    lua_pushnil(L);
    lua_pushlstring(L, message.c_str(), message.size());
    lua_pushinteger(L, code);
  }

  void initialize_error(lua_State* L) {
    luaX_set_field(L, -1, "strerror", impl_strerror);
    luaX_set_field(L, -1, "set_errno", impl_set_errno);
    luaX_set_field(L, -1, "get_errno", impl_get_errno);

    luaX_set_field(L, -1, "EAGAIN", EAGAIN);
    luaX_set_field(L, -1, "ECHILD", ECHILD);
    luaX_set_field(L, -1, "EINPROGRESS", EINPROGRESS);
    luaX_set_field(L, -1, "EINTR", EINTR);
    luaX_set_field(L, -1, "ENOENT", ENOENT);
    luaX_set_field(L, -1, "EPIPE", EPIPE);
    luaX_set_field(L, -1, "EWOULDBLOCK", EWOULDBLOCK);
  }
}
