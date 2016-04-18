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

#include <netdb.h>

#include <vector>

#include "common.hpp"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifndef NI_MAXSERV
#define NI_MAXSERV 32
#endif

namespace dromozoa {
  namespace {
    void push_netdb_error(lua_State* L, int code) {
      luaX_push(L, luaX_nil);
      if (const char* what = gai_strerror(code)) {
        luaX_push(L, what);
      } else {
        lua_pushfstring(L, "error number %d", code);
      }
      luaX_push(L, code);
    }

    void impl_getaddrinfo(lua_State* L) {
      const char* nodename = lua_tostring(L, 1);
      const char* servname = lua_tostring(L, 2);
      struct addrinfo* result = 0;
      int code = 0;
      if (lua_isnoneornil(L, 3)) {
        code = getaddrinfo(nodename, servname, 0, &result);
      } else {
        struct addrinfo hints = {};
        hints.ai_flags = luaX_opt_integer_field<int>(L, 3, "ai_flags", AI_V4MAPPED | AI_ADDRCONFIG);
        hints.ai_family = luaX_opt_integer_field<int>(L, 3, "ai_family", AF_UNSPEC);
        hints.ai_socktype = luaX_opt_integer_field<int>(L, 3, "ai_socktype", 0);
        hints.ai_protocol = luaX_opt_integer_field<int>(L, 3, "ai_protocol", 0);
        code = getaddrinfo(nodename, servname, &hints, &result);
      }
      if (code == 0) {
        lua_newtable(L);
        int i = 1;
        for (const struct addrinfo* ai = result; ai; ai = ai->ai_next, ++i) {
          lua_newtable(L);
          luaX_set_field(L, -1, "ai_family", ai->ai_family);
          luaX_set_field(L, -1, "ai_socktype", ai->ai_socktype);
          luaX_set_field(L, -1, "ai_protocol", ai->ai_protocol);
          luaX_set_field(L, -1, "ai_addrlen", ai->ai_addrlen);
          if (ai->ai_addr) {
            new_sockaddr(L, ai->ai_addr, ai->ai_addrlen);
            luaX_set_field(L, -2, "ai_addr");
          }
          if (ai->ai_canonname) {
            luaX_set_field(L, -1, "ai_canonname", ai->ai_canonname);
          }
          luaX_set_field(L, -2, i);
        }
        freeaddrinfo(result);
      } else {
        push_netdb_error(L, code);
      }
    }

    void impl_getnameinfo(lua_State* L) {
      const socket_address* address = check_sockaddr(L, 1);
      int flags = luaX_opt_integer<int>(L, 2, 0);
      std::vector<char> nodename(NI_MAXHOST);
      std::vector<char> servname(NI_MAXSERV);
      int code = getnameinfo(address->get(), address->size(), &nodename[0], nodename.size(), &servname[0], servname.size(), flags);
      if (code == 0) {
        luaX_push(L, &nodename[0]);
        luaX_push(L, &servname[0]);
      } else {
        push_netdb_error(L, code);
      }
    }
  }

  void initialize_netdb(lua_State* L) {
    luaX_set_field(L, -1, "getaddrinfo", impl_getaddrinfo);

    luaX_set_field(L, -1, "AI_PASSIVE", AI_PASSIVE);
    luaX_set_field(L, -1, "AI_CANONNAME", AI_CANONNAME);
    luaX_set_field(L, -1, "AI_NUMERICHOST", AI_NUMERICHOST);
    luaX_set_field(L, -1, "AI_NUMERICSERV", AI_NUMERICSERV);
    luaX_set_field(L, -1, "AI_V4MAPPED", AI_V4MAPPED);
    luaX_set_field(L, -1, "AI_ALL", AI_ALL);
    luaX_set_field(L, -1, "AI_ADDRCONFIG", AI_ADDRCONFIG);

    luaX_set_field(L, -1, "NI_NOFQDN", NI_NOFQDN);
    luaX_set_field(L, -1, "NI_NUMERICHOST", NI_NUMERICHOST);
    luaX_set_field(L, -1, "NI_NAMEREQD", NI_NAMEREQD);
    luaX_set_field(L, -1, "NI_NUMERICSERV", NI_NUMERICSERV);
#ifdef NI_NUMERICSCOPE
    luaX_set_field(L, -1, "NI_NUMERICSCOPE", NI_NUMERICSCOPE);
#endif
    luaX_set_field(L, -1, "NI_DGRAM", NI_DGRAM);
  }

  void initialize_sockaddr_netdb(lua_State* L) {
    luaX_set_field(L, -1, "getnameinfo", impl_getnameinfo);
  }
}
