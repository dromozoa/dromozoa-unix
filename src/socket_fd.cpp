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
#include "sockaddr.hpp"
#include "socket_fd.hpp"

namespace dromozoa {
  namespace {
    struct sockaddr* sockaddr_cast(struct sockaddr_storage* address) {
      return reinterpret_cast<struct sockaddr*>(address);
    }

    int impl_getsockname(lua_State* L) {
      struct sockaddr_storage ss = {};
      socklen_t size = sizeof(ss);
      if (getsockname(get_fd(L, 1), sockaddr_cast(&ss), &size) == -1) {
        return push_error(L);
      } else {
        new_sockaddr(L, sockaddr_cast(&ss), size);
        return 1;
      }
    }

    int impl_getpeername(lua_State* L) {
      struct sockaddr_storage ss = {};
      socklen_t size = sizeof(ss);
      if (getpeername(get_fd(L, 1), sockaddr_cast(&ss), &size) == -1) {
        return push_error(L);
      } else {
        new_sockaddr(L, sockaddr_cast(&ss), size);
        return 1;
      }
    }
  }

  void initialize_socket_fd(lua_State* L) {
    function<impl_getsockname>::set_field(L, "getsockname");
    function<impl_getpeername>::set_field(L, "getpeername");
  }
}
