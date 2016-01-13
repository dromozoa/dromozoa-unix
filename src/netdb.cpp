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

#include <netdb.h>

#include <vector>

#include "dromozoa/bind.hpp"

#include "netdb.hpp"
#include "sockaddr.hpp"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifndef NI_MAXSERV
#define NI_MAXSERV 32
#endif

namespace dromozoa {
  using bind::function;
  using bind::set_field;

  namespace {
    int push_netdb_error(lua_State* L, int code) {
      lua_pushnil(L);
      if (const char* what = gai_strerror(code)) {
        lua_pushstring(L, what);
      } else {
        lua_pushfstring(L, "error number %d", code);
      }
      lua_pushinteger(L, code);
      return 3;
    }

    int impl_getaddrinfo(lua_State* L) {
      const char* nodename = lua_tostring(L, 1);
      const char* servname = lua_tostring(L, 2);
      struct addrinfo* result = 0;
      int code = 0;
      if (lua_isnoneornil(L, 3)) {
        code = getaddrinfo(nodename, servname, 0, &result);
      } else {
        struct addrinfo hints = {};
        lua_getfield(L, 3, "ai_flags");
        hints.ai_flags = luaL_optinteger(L, -1, AI_V4MAPPED | AI_ADDRCONFIG);
        lua_pop(L, 1);
        lua_getfield(L, 3, "ai_family");
        hints.ai_family = luaL_optinteger(L, -1, AF_UNSPEC);
        lua_pop(L, 1);
        lua_getfield(L, 3, "ai_socktype");
        hints.ai_socktype = luaL_optinteger(L, -1, 0);
        lua_pop(L, 1);
        lua_getfield(L, 3, "ai_protocol");
        hints.ai_protocol = luaL_optinteger(L, -1, 0);
        lua_pop(L, 1);
        code = getaddrinfo(nodename, servname, &hints, &result);
      }
      if (code == 0) {
        lua_newtable(L);
        int i = 1;
        for (struct addrinfo* ai = result; ai; ai = ai->ai_next, ++i) {
          lua_pushinteger(L, i);
          lua_newtable(L);
          set_field(L, "ai_family", ai->ai_family);
          set_field(L, "ai_socktype", ai->ai_socktype);
          set_field(L, "ai_protocol", ai->ai_protocol);
          set_field(L, "ai_addrlen", ai->ai_addrlen);
          if (ai->ai_addr) {
            new_sockaddr(L, ai->ai_addr, ai->ai_addrlen);
            lua_setfield(L, -2, "ai_addr");
          }
          if (ai->ai_canonname) {
            lua_pushstring(L, ai->ai_canonname);
            lua_setfield(L, -2, "ai_canonname");
          }
          lua_settable(L, -3);
        }
        freeaddrinfo(result);
        return 1;
      } else {
        return push_netdb_error(L, code);
      }
    }

    int impl_getnameinfo(lua_State* L) {
      std::vector<char> nodename(NI_MAXHOST);
      std::vector<char> servname(NI_MAXSERV);
      socklen_t size = 0;
      const struct sockaddr* address = get_sockaddr(L, 1, size);
      int flags = luaL_optinteger(L, 2, 0);
      int code = getnameinfo(address, size, &nodename[0], nodename.size(), &servname[0], servname.size(), flags);
      if (code == 0) {
        lua_pushstring(L, &nodename[0]);
        lua_pushstring(L, &servname[0]);
        return 2;
      } else {
        return push_netdb_error(L, code);
      }
    }
  }

  void initialize_netdb(lua_State* L) {
    function<impl_getaddrinfo>::set_field(L, "getaddrinfo");

    DROMOZOA_BIND_SET_FIELD(L, AI_PASSIVE);
    DROMOZOA_BIND_SET_FIELD(L, AI_CANONNAME);
    DROMOZOA_BIND_SET_FIELD(L, AI_NUMERICHOST);
    DROMOZOA_BIND_SET_FIELD(L, AI_NUMERICSERV);
    DROMOZOA_BIND_SET_FIELD(L, AI_V4MAPPED);
    DROMOZOA_BIND_SET_FIELD(L, AI_ALL);
    DROMOZOA_BIND_SET_FIELD(L, AI_ADDRCONFIG);

    DROMOZOA_BIND_SET_FIELD(L, NI_NOFQDN);
    DROMOZOA_BIND_SET_FIELD(L, NI_NUMERICHOST);
    DROMOZOA_BIND_SET_FIELD(L, NI_NAMEREQD);
    DROMOZOA_BIND_SET_FIELD(L, NI_NUMERICSERV);
#ifdef NI_NUMERICSCOPE
    DROMOZOA_BIND_SET_FIELD(L, NI_NUMERICSCOPE);
#endif
    DROMOZOA_BIND_SET_FIELD(L, NI_DGRAM);
  }

  void initialize_getnameinfo(lua_State* L) {
    function<impl_getnameinfo>::set_field(L, "getnameinfo");
  }
}
