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

#include "addrinfo.hpp"
#include "function.hpp"
#include "nameinfo.hpp"
#include "sockaddr.hpp"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifndef NI_MAXSERV
#define NI_MAXSERV 32
#endif

namespace dromozoa {
  namespace {
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
        return push_addrinfo_error(L, code);
      }
    }
  }

  void initialize_nameinfo(lua_State* L) {
    function<impl_getnameinfo>::set_field(L, "getnameinfo");
  }
}
