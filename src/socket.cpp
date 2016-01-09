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
}

#include <sys/socket.h>

#include "error.hpp"
#include "fd.hpp"
#include "function.hpp"
#include "set_field.hpp"
#include "socket.hpp"

namespace dromozoa {
  namespace {
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

  void initialize_socket(lua_State* L) {
    function<impl_socketpair>::set_field(L, "socketpair");

    DROMOZOA_SET_FIELD(L, AF_INET);
    DROMOZOA_SET_FIELD(L, AF_INET6);
    DROMOZOA_SET_FIELD(L, AF_UNIX);
    DROMOZOA_SET_FIELD(L, AF_UNSPEC);

    DROMOZOA_SET_FIELD(L, SOCK_STREAM);
    DROMOZOA_SET_FIELD(L, SOCK_DGRAM);
  }
}
