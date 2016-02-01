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
}

#include "dromozoa/bind.hpp"

#include "coe.hpp"
#include "error.hpp"
#include "fcntl.hpp"
#include "fd.hpp"
#include "forkexec.hpp"
#include "lock.hpp"
#include "ndelay.hpp"
#include "netdb.hpp"
#include "netinet.hpp"
#include "pipe.hpp"
#include "read.hpp"
#include "selector.hpp"
#include "selfpipe.hpp"
#include "signal.hpp"
#include "sockaddr.hpp"
#include "socket.hpp"
#include "stdlib.hpp"
#include "sys_socket.hpp"
#include "sys_stat.hpp"
#include "sys_time.hpp"
#include "sys_wait.hpp"
#include "time.hpp"
#include "unistd.hpp"
#include "write.hpp"

namespace dromozoa {
  int open_asio(lua_State* L) {
    lua_getglobal(L, "require");
    lua_pushliteral(L, "dromozoa.unix.asio");
    lua_call(L, 1, 1);
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "super");
    return 1;
  }

  int open_timespec(lua_State* L) {
    lua_getglobal(L, "require");
    lua_pushliteral(L, "dromozoa.unix.timespec");
    lua_call(L, 1, 1);
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "super");
    return 1;
  }

  int open(lua_State* L) {
    lua_newtable(L);

    open_fd(L);
    initialize_coe(L);
    initialize_lock(L);
    initialize_ndelay(L);
    initialize_read(L);
    initialize_write(L);
    initialize_socket(L);
    lua_setfield(L, -2, "fd");

    open_selector(L);
    lua_setfield(L, -2, "selector");

    open_selfpipe(L);
    lua_setfield(L, -2, "selfpipe");

    open_sockaddr(L);
    initialize_getnameinfo(L);
    lua_setfield(L, -2, "sockaddr");

    open_asio(L);
    lua_setfield(L, -2, "asio");

    open_timespec(L);
    lua_setfield(L, -2, "timespec");

    bind::initialize(L);
    initialize_error(L);
    initialize_fcntl(L);
    initialize_forkexec(L);
    initialize_netdb(L);
    initialize_netinet(L);
    initialize_pipe(L);
    initialize_signal(L);
    initialize_sockaddr(L);
    initialize_stdlib(L);
    initialize_sys_socket(L);
    initialize_sys_stat(L);
    initialize_sys_time(L);
    initialize_sys_wait(L);
    initialize_time(L);
    initialize_unistd(L);

    return 1;
  }
}

extern "C" int luaopen_dromozoa_unix(lua_State* L) {
  return dromozoa::open(L);
}
