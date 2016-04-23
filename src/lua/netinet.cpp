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

#include <netinet/in.h>
#include <netinet/tcp.h>

#include "common.hpp"

namespace dromozoa {
  void initialize_netinet(lua_State* L) {
    luaX_set_field<int>(L, -1, "IPPROTO_IP", IPPROTO_IP);
    luaX_set_field<int>(L, -1, "IPPROTO_IPV6", IPPROTO_IPV6);
    luaX_set_field<int>(L, -1, "IPPROTO_ICMP", IPPROTO_ICMP);
    luaX_set_field<int>(L, -1, "IPPROTO_RAW", IPPROTO_RAW);
    luaX_set_field<int>(L, -1, "IPPROTO_TCP", IPPROTO_TCP);
    luaX_set_field<int>(L, -1, "IPPROTO_UDP", IPPROTO_UDP);
    luaX_set_field(L, -1, "TCP_NODELAY", TCP_NODELAY);
  }
}
