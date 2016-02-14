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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include "dromozoa/bind.hpp"

#include "coe.hpp"
#include "error.hpp"
#include "fd.hpp"
#include "ndelay.hpp"
#include "sockaddr.hpp"
#include "socket.hpp"

namespace dromozoa {
  using bind::function;
  using bind::push_success;

  namespace {
#ifdef HAVE_ACCEPT4
    int wrap_accept4(int fd, struct sockaddr* address, socklen_t* size, int flags) {
      int f = 0;
      if (flags & O_CLOEXEC) {
        f |= SOCK_CLOEXEC;
      }
      if (flags & O_NONBLOCK) {
        f |= SOCK_NONBLOCK;
      }
      return accept4(fd, address, size, f);
    }
#else
    int wrap_accept4(int fd, struct sockaddr* address, socklen_t* size, int flags) {
      int result = accept(fd, address, size);
      if (result == -1) {
        return -1;
      }
      do {
        if (flags & O_CLOEXEC) {
          if (coe(result) == -1) {
            break;
          }
        }
        if (flags & O_NONBLOCK) {
          if (ndelay_on(result) == -1) {
            break;
          }
        }
        return result;
      } while (false);
      int code = errno;
      close(result);
      errno = code;
      return -1;
    }
#endif

    struct sockaddr* sockaddr_cast(struct sockaddr_storage* address) {
      return reinterpret_cast<struct sockaddr*>(address);
    }

    int impl_getsockname(lua_State* L) {
      struct sockaddr_storage ss = {};
      socklen_t size = sizeof(ss);
      if (getsockname(get_fd(L, 1), sockaddr_cast(&ss), &size) == -1) {
        return push_error(L);
      } else {
        return new_sockaddr(L, sockaddr_cast(&ss), size);
      }
    }

    int impl_getpeername(lua_State* L) {
      struct sockaddr_storage ss = {};
      socklen_t size = sizeof(ss);
      if (getpeername(get_fd(L, 1), sockaddr_cast(&ss), &size) == -1) {
        return push_error(L);
      } else {
        return new_sockaddr(L, sockaddr_cast(&ss), size);
      }
    }

    int impl_bind(lua_State* L) {
      socklen_t size = 0;
      const struct sockaddr* address = get_sockaddr(L, 2, size);
      if (::bind(get_fd(L, 1), address, size) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_listen(lua_State* L) {
      int backlog = luaL_optinteger(L, 2, SOMAXCONN);
      if (listen(get_fd(L, 1), backlog) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_accept(lua_State* L) {
      int flags = luaL_optinteger(L, 2, 0);
      struct sockaddr_storage ss = {};
      socklen_t size = sizeof(ss);
      int result = wrap_accept4(get_fd(L, 1), sockaddr_cast(&ss), &size, flags);
      if (result == -1) {
        int code = errno;
        if (code == EAGAIN || code == EWOULDBLOCK) {
          return push_resource_unavailable_try_again(L);
        } else {
          return push_error(L, code);
        }
      } else {
        new_fd(L, result);
        new_sockaddr(L, sockaddr_cast(&ss), size);
        return 2;
      }
    }

    int impl_connect(lua_State* L) {
      socklen_t size = 0;
      const struct sockaddr* address = get_sockaddr(L, 2, size);
      if (connect(get_fd(L, 1), address, size) == -1) {
        int code = errno;
        if (code == EINPROGRESS) {
          return push_operation_in_progress(L);
        } else {
          return push_error(L, code);
        }
      } else {
        return push_success(L);
      }
    }

    int impl_shutdown(lua_State* L) {
      int how = luaL_checkinteger(L, 2);
      if (shutdown(get_fd(L, 1), how) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_setsockopt(lua_State* L) {
      int level = luaL_checkinteger(L, 2);
      int name = luaL_checkinteger(L, 3);
      int value = luaL_checkinteger(L, 4);
      if (setsockopt(get_fd(L, 1), level, name, &value, sizeof(value)) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_getsockopt(lua_State* L) {
      int level = luaL_checkinteger(L, 2);
      int name = luaL_checkinteger(L, 3);
      int value = 0;
      socklen_t size = sizeof(value);
      if (getsockopt(get_fd(L, 1), level, name, &value, &size) == -1) {
        return push_error(L);
      } else {
        lua_pushinteger(L, value);
        return 1;
      }
    }
  }

  void initialize_socket(lua_State* L) {
    function<impl_getsockname>::set_field(L, "getsockname");
    function<impl_getpeername>::set_field(L, "getpeername");
    function<impl_bind>::set_field(L, "bind");
    function<impl_listen>::set_field(L, "listen");
    function<impl_accept>::set_field(L, "accept");
    function<impl_connect>::set_field(L, "connect");
    function<impl_shutdown>::set_field(L, "shutdown");
    function<impl_setsockopt>::set_field(L, "setsockopt");
    function<impl_getsockopt>::set_field(L, "getsockopt");
  }
}
