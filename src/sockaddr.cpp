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

#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "function.hpp"
#include "sockaddr.hpp"

namespace dromozoa {
  namespace {
    struct socket_address {
      socklen_t size;
      struct sockaddr address[1];
    };
  }

  int new_sockaddr(lua_State* L, const struct sockaddr* address, socklen_t size) {
    socket_address* data = static_cast<socket_address*>(lua_newuserdata(L, offsetof(socket_address, address) + size));
    data->size = size;
    memcpy(data->address, address, size);
    luaL_getmetatable(L, "dromozoa.unix.sockaddr");
    lua_setmetatable(L, -2);
    return 1;
  }

  const sockaddr* get_sockaddr(lua_State* L, int n, socklen_t& size) {
    socket_address* data = static_cast<socket_address*>(luaL_checkudata(L, n, "dromozoa.unix.sockaddr"));
    size = data->size;
    return data->address;
  }

  namespace {
    namespace {
      socklen_t get_size(lua_State* L, int n) {
        return static_cast<const socket_address*>(luaL_checkudata(L, n, "dromozoa.unix.sockaddr"))->size;
      }

      const struct sockaddr* get_address(lua_State* L, int n) {
        return static_cast<const socket_address*>(luaL_checkudata(L, n, "dromozoa.unix.sockaddr"))->address;
      }
    }

    int impl_size(lua_State* L) {
      lua_pushinteger(L, get_size(L, 1));
      return 1;
    }

    int impl_family(lua_State* L) {
      lua_pushinteger(L, get_address(L, 1)->sa_family);
      return 1;
    }

    int impl_path(lua_State* L) {
      socklen_t size = 0;
      const struct sockaddr* address = get_sockaddr(L, 1, size);
      if (address->sa_family == AF_UNIX) {
        const struct sockaddr_un* sun = reinterpret_cast<const struct sockaddr_un*>(address);
        socklen_t n = size - offsetof(struct sockaddr_un, sun_path);
        if (n > 0) {
          const char* path = sun->sun_path;
          ssize_t i = 0;
          // abstract socket address
          if (n > 1 && path[0] == '\0' && path[1] != '\0') {
            i = 1;
          }
          for (; i < n && path[i] != '\0'; ++i) {}
          lua_pushlstring(L, path, i);
          return 1;
        } else {
          lua_pushliteral(L, "");
          return 1;
        }
      } else {
        lua_pushnil(L);
        return 1;
      }
    }

    int impl_sockaddr_un(lua_State* L) {
      size_t size = 0;
      const char* path = luaL_checklstring(L, 1, &size);
      struct sockaddr_un sun = {};
      sun.sun_family = AF_UNIX;
      if (size < sizeof(sun.sun_path)) {
        memcpy(sun.sun_path, path, size);
        sun.sun_path[size] = '\0';
        new_sockaddr(L, reinterpret_cast<const struct sockaddr*>(&sun), sizeof(sun));
        return 1;
      } else {
        lua_pushnil(L);
        return 1;
      }
    }
  }

  int open_sockaddr(lua_State* L) {
    lua_newtable(L);
    function<impl_size>::set_field(L, "size");
    function<impl_family>::set_field(L, "family");
    function<impl_path>::set_field(L, "path");
    luaL_newmetatable(L, "dromozoa.unix.sockaddr");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
    return 1;
  }

  void initialize_sockaddr(lua_State* L) {
    function<impl_sockaddr_un>::set_field(L, "sockaddr_un");
  }
}
