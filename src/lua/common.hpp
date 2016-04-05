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

#include <sys/socket.h>

#include <dromozoa/bind.hpp>

#include <dromozoa/argument_vector.hpp>
#include <dromozoa/socket_address.hpp>

namespace dromozoa {
  int push_resource_unavailable_try_again(lua_State* L);
  int push_operation_in_progress(lua_State* L);
  int push_interrupted(lua_State* L);
  int push_broken_pipe(lua_State* L);
  int push_timed_out(lua_State* L);

  int push_error(lua_State* L);
  int push_error(lua_State* L, int code);

  argument_vector make_argument_vector(lua_State* L, int n);

  void new_fd(lua_State* L, int fd);
  int check_fd(lua_State* L, int index);

  void new_sockaddr(lua_State* L, const socket_address& address);
  void new_sockaddr(lua_State* L, const struct sockaddr* address, socklen_t size);
  const socket_address* get_sockaddr(lua_State* L, int n);
}

#endif
