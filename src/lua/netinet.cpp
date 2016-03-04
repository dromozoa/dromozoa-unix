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

#include <netinet/in.h>
#include <netinet/tcp.h>

#include "dromozoa/bind.hpp"

#include "netinet.hpp"

namespace dromozoa {
  void initialize_netinet(lua_State* L) {
    DROMOZOA_BIND_SET_FIELD(L, IPPROTO_IP);
    DROMOZOA_BIND_SET_FIELD(L, IPPROTO_IPV6);
    DROMOZOA_BIND_SET_FIELD(L, IPPROTO_ICMP);
    DROMOZOA_BIND_SET_FIELD(L, IPPROTO_RAW);
    DROMOZOA_BIND_SET_FIELD(L, IPPROTO_TCP);
    DROMOZOA_BIND_SET_FIELD(L, IPPROTO_UDP);
    DROMOZOA_BIND_SET_FIELD(L, TCP_NODELAY);
  }
}
