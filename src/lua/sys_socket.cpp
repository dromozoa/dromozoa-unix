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

#include <sys/socket.h>

#include <dromozoa/compat_socket.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_socket(lua_State* L) {
      int domain = luaX_check_integer<int>(L, 1);
      int type = luaX_check_integer<int>(L, 2);
      int protocol = luaX_opt_integer<int>(L, 3, 0);
      int result = compat_socket(domain, type, protocol);
      if (result == -1) {
        push_error(L);
      } else {
        new_fd(L, result);
      }
    }

    void impl_socketpair(lua_State* L) {
      int domain = luaX_check_integer<int>(L, 1);
      int type = luaX_check_integer<int>(L, 2);
      int protocol = luaX_opt_integer<int>(L, 3, 0);
      int fd[2] = { -1, -1 };
      if (compat_socketpair(domain, type, protocol, fd) == -1) {
        push_error(L);
      } else {
        new_fd(L, fd[0]);
        new_fd(L, fd[1]);
      }
    }
  }

  void initialize_sys_socket(lua_State* L) {
    luaX_set_field(L, -1, "socket", impl_socket);
    luaX_set_field(L, -1, "socketpair", impl_socketpair);

    luaX_set_field(L, -1, "AF_INET", AF_INET);
    luaX_set_field(L, -1, "AF_INET6", AF_INET6);
    luaX_set_field(L, -1, "AF_UNIX", AF_UNIX);
    luaX_set_field(L, -1, "AF_UNSPEC", AF_UNSPEC);

    luaX_set_field<int>(L, -1, "SOCK_STREAM", SOCK_STREAM);
    luaX_set_field<int>(L, -1, "SOCK_DGRAM", SOCK_DGRAM);
    luaX_set_field<int>(L, -1, "SOCK_SEQPACKET", SOCK_SEQPACKET);
    luaX_set_field(L, -1, "SOCK_CLOEXEC", COMPAT_SOCK_CLOEXEC);
    luaX_set_field(L, -1, "SOCK_NONBLOCK", COMPAT_SOCK_NONBLOCK);

    luaX_set_field(L, -1, "SOMAXCONN", SOMAXCONN);

    luaX_set_field<int>(L, -1, "SHUT_RD", SHUT_RD);
    luaX_set_field<int>(L, -1, "SHUT_WR", SHUT_WR);
    luaX_set_field<int>(L, -1, "SHUT_RDWR", SHUT_RDWR);

    luaX_set_field(L, -1, "SOL_SOCKET", SOL_SOCKET);
    luaX_set_field(L, -1, "SO_BROADCAST", SO_BROADCAST);
    luaX_set_field(L, -1, "SO_ERROR", SO_ERROR);
    luaX_set_field(L, -1, "SO_RCVBUF", SO_RCVBUF);
    luaX_set_field(L, -1, "SO_REUSEADDR", SO_REUSEADDR);
    luaX_set_field(L, -1, "SO_SNDBUF", SO_SNDBUF);

    luaX_set_field<int>(L, -1, "MSG_EOR", MSG_EOR);
    luaX_set_field<int>(L, -1, "MSG_OOB", MSG_OOB);
    luaX_set_field<int>(L, -1, "MSG_PEEK", MSG_PEEK);
    luaX_set_field<int>(L, -1, "MSG_WAITALL", MSG_WAITALL);
  }
}
