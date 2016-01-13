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

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <sys/socket.h>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "fd.hpp"
#include "sys_socket.hpp"

namespace dromozoa {
  using bind::function;

  namespace {
    int impl_socket(lua_State* L) {
      int domain = luaL_checkinteger(L, 1);
      int type = luaL_checkinteger(L, 2);
      int protocol = luaL_optinteger(L, 3, 0);
      int result = socket(domain, type, protocol);
      if (result == -1) {
        return push_error(L);
      } else {
        new_fd(L, result);
        return 1;
      }
    }

    int impl_socketpair(lua_State* L) {
      int domain = luaL_checkinteger(L, 1);
      int type = luaL_checkinteger(L, 2);
      int protocol = luaL_optinteger(L, 3, 0);
      int fd[2] = { -1, -1 };
      if (socketpair(domain, type, protocol, fd) == -1) {
        return push_error(L);
      } else {
        new_fd(L, fd[0]);
        new_fd(L, fd[1]);
        return 2;
      }
    }
  }

  void initialize_sys_socket(lua_State* L) {
    function<impl_socket>::set_field(L, "socket");
    function<impl_socketpair>::set_field(L, "socketpair");

    DROMOZOA_BIND_SET_FIELD(L, AF_INET);
    DROMOZOA_BIND_SET_FIELD(L, AF_INET6);
    DROMOZOA_BIND_SET_FIELD(L, AF_UNIX);
    DROMOZOA_BIND_SET_FIELD(L, AF_UNSPEC);

    DROMOZOA_BIND_SET_FIELD(L, SOCK_STREAM);
    DROMOZOA_BIND_SET_FIELD(L, SOCK_DGRAM);

    DROMOZOA_BIND_SET_FIELD(L, SOMAXCONN);

    DROMOZOA_BIND_SET_FIELD(L, SHUT_RD);
    DROMOZOA_BIND_SET_FIELD(L, SHUT_WR);
    DROMOZOA_BIND_SET_FIELD(L, SHUT_RDWR);

    DROMOZOA_BIND_SET_FIELD(L, SOL_SOCKET);
    DROMOZOA_BIND_SET_FIELD(L, SO_ERROR);
    DROMOZOA_BIND_SET_FIELD(L, SO_RCVBUF);
    DROMOZOA_BIND_SET_FIELD(L, SO_REUSEADDR);
    DROMOZOA_BIND_SET_FIELD(L, SO_SNDBUF);
  }
}
