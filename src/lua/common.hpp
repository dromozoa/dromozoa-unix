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

#ifndef COMMON_HPP
#define COMMON_HPP

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <errno.h>
#include <sys/socket.h>

#include <dromozoa/bind.hpp>
#include <dromozoa/luacxx.hpp>
#include <dromozoa/argument_vector.hpp>
#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/socket_address.hpp>

#include "bind.hpp"

namespace dromozoa {
  // using bind::function;
  using bind::push_success;
  // using bind::set_field;
  using bind::translate_range_i;
  using bind::translate_range_j;

  class luaX_State_Unix : public luaX_State {
  public:
    explicit luaX_State_Unix(lua_State* L);

    luaX_State_Unix& push_failure(int code = errno) {
      push(luaX_nil, compat_strerror(code), code);
      return *this;
    }
  };

  namespace luacxx {
    inline int luaX_closure(lua_State* L, void (*function)(luaX_State_Unix&)) {
      int top = lua_gettop(L);
      luaX_State_Unix LX(L);
      function(LX);
      return lua_gettop(L) - top;
    }
  }

  int push_resource_unavailable_try_again(lua_State* L);
  int push_operation_in_progress(lua_State* L);
  int push_interrupted(lua_State* L);
  int push_broken_pipe(lua_State* L);
  int push_timed_out(lua_State* L);
  int push_error(lua_State* L, int code = errno);

  argument_vector make_argument_vector(lua_State* L, int n);

  int new_fd(lua_State* L, int fd, bool ref = false);
  int get_fd(lua_State* L, int n);

  int new_sockaddr(lua_State* L, const socket_address& address);
  int new_sockaddr(lua_State* L, const struct sockaddr* address, socklen_t size);
  const socket_address* get_sockaddr(lua_State* L, int n);
}

#endif
