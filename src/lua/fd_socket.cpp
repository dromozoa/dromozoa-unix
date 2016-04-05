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

#include <dromozoa/compat_accept4.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_bind(lua_State* L) {
      const socket_address* address = get_sockaddr(L, 2);
      if (::bind(get_fd(L, 1), address->get(), address->size()) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_listen(lua_State* L) {
      int backlog = luaL_optinteger(L, 2, SOMAXCONN);
      if (listen(get_fd(L, 1), backlog) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_accept(lua_State* L) {
      int flags = luaL_optinteger(L, 2, COMPAT_SOCK_CLOEXEC);
      socket_address address;
      int result = compat_accept4(get_fd(L, 1), address.get(), address.size_ptr(), flags);
      if (result == -1) {
        push_error(L);
      } else {
        new_fd(L, result);
        new_sockaddr(L, address);
      }
    }

    void impl_connect(lua_State* L) {
      const socket_address* address = get_sockaddr(L, 2);
      if (connect(get_fd(L, 1), address->get(), address->size()) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_shutdown(lua_State* L) {
      int how = luaL_checkinteger(L, 2);
      if (shutdown(get_fd(L, 1), how) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_setsockopt(lua_State* L) {
      int level = luaL_checkinteger(L, 2);
      int name = luaL_checkinteger(L, 3);
      int value = luaL_checkinteger(L, 4);
      if (setsockopt(get_fd(L, 1), level, name, &value, sizeof(value)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_getsockopt(lua_State* L) {
      int level = luaL_checkinteger(L, 2);
      int name = luaL_checkinteger(L, 3);
      int value;
      socklen_t size = sizeof(value);
      if (getsockopt(get_fd(L, 1), level, name, &value, &size) == -1) {
        push_error(L);
      } else {
        lua_pushinteger(L, value);
      }
    }

    void impl_getsockname(lua_State* L) {
      socket_address address;
      if (getsockname(get_fd(L, 1), address.get(), address.size_ptr()) == -1) {
        push_error(L);
      } else {
        new_sockaddr(L, address);
      }
    }

    void impl_getpeername(lua_State* L) {
      socket_address address;
      if (getpeername(get_fd(L, 1), address.get(), address.size_ptr()) == -1) {
        push_error(L);
      } else {
        new_sockaddr(L, address);
      }
    }
  }

  void initialize_fd_socket(lua_State* L) {
    luaX_set_field(L, "bind", impl_bind);
    luaX_set_field(L, "listen", impl_listen);
    luaX_set_field(L, "accept", impl_accept);
    luaX_set_field(L, "connect", impl_connect);
    luaX_set_field(L, "shutdown", impl_shutdown);
    luaX_set_field(L, "setsockopt", impl_setsockopt);
    luaX_set_field(L, "getsockopt", impl_getsockopt);
    luaX_set_field(L, "getsockname", impl_getsockname);
    luaX_set_field(L, "getpeername", impl_getpeername);
  }
}
