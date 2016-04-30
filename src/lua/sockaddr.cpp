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

#include <string.h>
#include <sys/un.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_size(lua_State* L) {
      luaX_push(L, check_sockaddr(L, 1)->size());
    }

    void impl_family(lua_State* L) {
      luaX_push(L, check_sockaddr(L, 1)->family());
    }

    void impl_path(lua_State* L) {
      const socket_address* self = check_sockaddr(L, 1);
      if (self->family() == AF_UNIX) {
        const struct sockaddr_un* sun = reinterpret_cast<const struct sockaddr_un*>(self->get());
        socklen_t arg = self->size() - offsetof(struct sockaddr_un, sun_path);
        if (arg > 0) {
          const char* path = sun->sun_path;
          socklen_t i = 0;
          // abstract socket address
          if (arg > 1 && path[0] == '\0' && path[1] != '\0') {
            i = 1;
          }
          for (; i < arg && path[i] != '\0'; ++i) {}
          lua_pushlstring(L, path, i);
        } else {
          luaX_push(L, "");
        }
      } else {
        luaX_push(L, luaX_nil);
      }
    }

    void impl_sockaddr_un(lua_State* L) {
      size_t size = 0;
      const char* path = luaL_checklstring(L, 1, &size);
      struct sockaddr_un sun = {};
      sun.sun_family = AF_UNIX;
      if (size < sizeof(sun.sun_path)) {
        memcpy(sun.sun_path, path, size);
        sun.sun_path[size] = '\0';
        new_sockaddr(L, reinterpret_cast<const struct sockaddr*>(&sun), sizeof(sun));
      } else {
        luaL_argerror(L, 1, "string too long");
      }
    }
  }

  void new_sockaddr(lua_State* L, const socket_address& address) {
    luaX_new<socket_address>(L, address);
    luaX_set_metatable(L, "dromozoa.unix.sockaddr");
  }

  void new_sockaddr(lua_State* L, const struct sockaddr* address, socklen_t size) {
    luaX_new<socket_address>(L, address, size);
    luaX_set_metatable(L, "dromozoa.unix.sockaddr");
  }

  const socket_address* check_sockaddr(lua_State* L, int arg) {
    return luaX_check_udata<socket_address>(L, arg, "dromozoa.unix.sockaddr");
  }

  void initialize_sockaddr_netdb(lua_State* L);

  void initialize_sockaddr(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.sockaddr");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      lua_pop(L, 1);

      luaX_set_field(L, -1, "size", impl_size);
      luaX_set_field(L, -1, "family", impl_family);
      luaX_set_field(L, -1, "path", impl_path);

      initialize_sockaddr_netdb(L);
    }
    luaX_set_field(L, -2, "sockaddr");

    luaX_set_field(L, -1, "sockaddr_un", impl_sockaddr_un);
  }
}
