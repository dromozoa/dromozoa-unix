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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include <dromozoa/compat_strerror.hpp>

#include "common.hpp"

namespace dromozoa {
  int push_resource_unavailable_try_again(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EAGAIN));
    return 1;
  }

  int push_operation_in_progress(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EINPROGRESS));
    return 1;
  }

  int push_interrupted(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EINTR));
    return 1;
  }

  int push_broken_pipe(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EPIPE));
    return 1;
  }

  int push_timed_out(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(ETIMEDOUT));
    return 1;
  }

  int push_error(lua_State* L, int code) {
    int save = errno;
    std::string message = compat_strerror(code);
    lua_pushnil(L);
    lua_pushlstring(L, message.c_str(), message.size());
    lua_pushinteger(L, code);
    errno = save;
    return 3;
  }

  namespace {
    int impl_strerror(lua_State* L) {
      int save = errno;
      int code = luaL_optinteger(L, 1, save);
      std::string message = compat_strerror(code);
      lua_pushlstring(L, message.c_str(), message.size());
      errno = save;
      return 1;
    }

    void impl_set_errno(lua_State* L) {
      int code = luaL_checkinteger(L, 1);
      errno = code;
      luaX_push_success(L);
    }

    int impl_get_errno(lua_State* L) {
      int code = errno;
      lua_pushinteger(L, code);
      return 1;
    }
  }

  void initialize_error(lua_State* L) {
    luaX_set_field(L, "strerror", impl_strerror);
    luaX_set_field(L, "set_errno", impl_set_errno);
    luaX_set_field(L, "get_errno", impl_get_errno);

    luaX_set_field(L, "EAGAIN", EAGAIN);
    luaX_set_field(L, "EINPROGRESS", EINPROGRESS);
    luaX_set_field(L, "EINTR", EINTR);
    luaX_set_field(L, "ENOENT", ENOENT);
    luaX_set_field(L, "EPIPE", EPIPE);
    luaX_set_field(L, "ETIMEDOUT", ETIMEDOUT);
    luaX_set_field(L, "EWOULDBLOCK", EWOULDBLOCK);

    push_resource_unavailable_try_again(L);
    lua_setfield(L, -2, "resource_unavailable_try_again");

    push_operation_in_progress(L);
    lua_setfield(L, -2, "operation_in_progress");

    push_interrupted(L);
    lua_setfield(L, -2, "interrupted");

    push_broken_pipe(L);
    lua_setfield(L, -2, "broken_pipe");

    push_timed_out(L);
    lua_setfield(L, -2, "timed_out");
  }
}
