// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <vector>

#include <dromozoa/compat_accept4.hpp>
#include <dromozoa/compat_socket.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_bind(lua_State* L) {
      const socket_address* address = check_sockaddr(L, 2);
      if (::bind(check_fd(L, 1), address->get(), address->size()) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_listen(lua_State* L) {
      int backlog = luaX_opt_integer<int>(L, 2, SOMAXCONN);
      if (listen(check_fd(L, 1), backlog) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_accept(lua_State* L) {
      int flags = luaX_opt_integer<int>(L, 2, COMPAT_SOCK_CLOEXEC);
      socket_address address;
      int result = compat_accept4(check_fd(L, 1), address.get(), address.size_ptr(), flags);
      if (result == -1) {
        push_error(L);
      } else {
        new_fd(L, result);
        new_sockaddr(L, address);
      }
    }

    void impl_connect(lua_State* L) {
      const socket_address* address = check_sockaddr(L, 2);
      if (connect(check_fd(L, 1), address->get(), address->size()) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_shutdown(lua_State* L) {
      int how = luaX_check_integer<int>(L, 2);
      if (shutdown(check_fd(L, 1), how) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_setsockopt(lua_State* L) {
      int level = luaX_check_integer<int>(L, 2);
      int name = luaX_check_integer<int>(L, 3);
      int value = luaX_check_integer<int>(L, 4);
      if (setsockopt(check_fd(L, 1), level, name, &value, sizeof(value)) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_getsockopt(lua_State* L) {
      int level = luaX_check_integer<int>(L, 2);
      int name = luaX_check_integer<int>(L, 3);
      int value = 0;
      socklen_t size = sizeof(value);
      if (getsockopt(check_fd(L, 1), level, name, &value, &size) == -1) {
        push_error(L);
      } else {
        luaX_push(L, value);
      }
    }

    void impl_getsockname(lua_State* L) {
      socket_address address;
      if (getsockname(check_fd(L, 1), address.get(), address.size_ptr()) == -1) {
        push_error(L);
      } else {
        new_sockaddr(L, address);
      }
    }

    void impl_getpeername(lua_State* L) {
      socket_address address;
      if (getpeername(check_fd(L, 1), address.get(), address.size_ptr()) == -1) {
        push_error(L);
      } else {
        new_sockaddr(L, address);
      }
    }

    void impl_recv(lua_State* L) {
      int fd = check_fd(L, 1);
      size_t size = luaX_check_integer<size_t>(L, 2);
      int flags = luaX_opt_integer<int>(L, 3, 0);
      std::vector<char> buffer(size);
      ssize_t result = recv(fd, &buffer[0], buffer.size(), flags);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, luaX_string_reference(&buffer[0], result));
      }
    }

    void impl_recvfrom(lua_State* L) {
      int fd = check_fd(L, 1);
      size_t size = luaX_check_integer<size_t>(L, 2);
      int flags = luaX_opt_integer<int>(L, 3, 0);
      std::vector<char> buffer(size);
      socket_address address;
      ssize_t result = recvfrom(fd, &buffer[0], buffer.size(), flags, address.get(), address.size_ptr());
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, luaX_string_reference(&buffer[0], result));
        new_sockaddr(L, address);
      }
    }

    void impl_send(lua_State* L) {
      luaX_string_reference buffer = luaX_check_string(L, 2);
      size_t i = luaX_opt_range_i(L, 3, buffer.size());
      size_t j = luaX_opt_range_j(L, 4, buffer.size());
      if (j < i) {
        j = i;
      }
      int flags = luaX_opt_integer<int>(L, 5, 0);
      ssize_t result = send(check_fd(L, 1), buffer.data() + i, j - i, flags);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, result);
      }
    }

    void impl_sendto(lua_State* L) {
      luaX_string_reference buffer = luaX_check_string(L, 2);
      size_t i = luaX_opt_range_i(L, 3, buffer.size());
      size_t j = luaX_opt_range_j(L, 4, buffer.size());
      if (j < i) {
        j = i;
      }
      int flags = luaX_opt_integer<int>(L, 5, 0);
      const struct sockaddr* dest_addr = 0;
      socklen_t dest_len = 0;
      if (!lua_isnoneornil(L, 6)) {
        const socket_address* address = check_sockaddr(L, 6);
        dest_addr = address->get();
        dest_len = address->size();
      }
      ssize_t result = sendto(check_fd(L, 1), buffer.data() + i, j - i, flags, dest_addr, dest_len);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, result);
      }
    }
  }

  void initialize_fd_socket(lua_State* L) {
    luaX_set_field(L, -1, "bind", impl_bind);
    luaX_set_field(L, -1, "listen", impl_listen);
    luaX_set_field(L, -1, "accept", impl_accept);
    luaX_set_field(L, -1, "connect", impl_connect);
    luaX_set_field(L, -1, "shutdown", impl_shutdown);
    luaX_set_field(L, -1, "setsockopt", impl_setsockopt);
    luaX_set_field(L, -1, "getsockopt", impl_getsockopt);
    luaX_set_field(L, -1, "getsockname", impl_getsockname);
    luaX_set_field(L, -1, "getpeername", impl_getpeername);
    luaX_set_field(L, -1, "recv", impl_recv);
    luaX_set_field(L, -1, "recvfrom", impl_recvfrom);
    luaX_set_field(L, -1, "send", impl_send);
    luaX_set_field(L, -1, "sendto", impl_sendto);
  }
}
