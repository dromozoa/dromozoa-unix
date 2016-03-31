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

#include <new>

#include <dromozoa/socket_address.hpp>

#include "common.hpp"

namespace dromozoa {
  int new_sockaddr(lua_State* L, const socket_address& address) {
    socket_address* self = static_cast<socket_address*>(lua_newuserdata(L, sizeof(socket_address)));
    new(self) socket_address(address);
    luaL_getmetatable(L, "dromozoa.unix.sockaddr");
    lua_setmetatable(L, -2);
    return 1;
  }

  int new_sockaddr(lua_State* L, const struct sockaddr* address, socklen_t size) {
    socket_address* self = static_cast<socket_address*>(lua_newuserdata(L, sizeof(socket_address)));
    new(self) socket_address(address, size);
    luaL_getmetatable(L, "dromozoa.unix.sockaddr");
    lua_setmetatable(L, -2);
    return 1;
  }

  const socket_address* get_sockaddr(lua_State* L, int n) {
    return static_cast<socket_address*>(luaL_checkudata(L, n, "dromozoa.unix.sockaddr"));
  }

  namespace {
    int impl_size(lua_State* L) {
      lua_pushinteger(L, get_sockaddr(L, 1)->size());
      return 1;
    }

    int impl_family(lua_State* L) {
      lua_pushinteger(L, get_sockaddr(L, 1)->family());
      return 1;
    }

    int impl_path(lua_State* L) {
      const socket_address* self = get_sockaddr(L, 1);
      if (self->family() == AF_UNIX) {
        const struct sockaddr_un* sun = reinterpret_cast<const struct sockaddr_un*>(self->get());
        socklen_t n = self->size() - offsetof(struct sockaddr_un, sun_path);
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
        // [TODO] error?
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
        return new_sockaddr(L, reinterpret_cast<const struct sockaddr*>(&sun), sizeof(sun));
      } else {
        // [TODO] error?
        lua_pushnil(L);
        return 1;
      }
    }
  }

  int open_sockaddr(lua_State* L) {
    lua_newtable(L);
    luaX_set_field(L, "size", impl_size);
    luaX_set_field(L, "family", impl_family);
    luaX_set_field(L, "path", impl_path);

    luaL_newmetatable(L, "dromozoa.unix.sockaddr");
    lua_pushvalue(L, -2);
    luaX_set_field(L, "__index");
    lua_pop(L, 1);

    return 1;
  }

  void initialize_sockaddr(lua_State* L) {
    luaX_set_field(L, "sockaddr_un", impl_sockaddr_un);
  }
}
