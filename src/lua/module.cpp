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

#include "common.hpp"

namespace dromozoa {
  void initialize_async_service(lua_State* L);
  void initialize_async_task(lua_State* L);
  void initialize_errno(lua_State* L);
  void initialize_error(lua_State* L);
  void initialize_fcntl(lua_State* L);
  void initialize_fd(lua_State* L);
  void initialize_netdb(lua_State* L);
  void initialize_netinet(lua_State* L);
  void initialize_pathexec(lua_State* L);
  void initialize_pipe(lua_State* L);
  void initialize_process(lua_State* L);
  void initialize_sched(lua_State* L);
  void initialize_selector(lua_State* L);
  void initialize_selfpipe(lua_State* L);
  void initialize_signal(lua_State* L);
  void initialize_sockaddr(lua_State* L);
  void initialize_stdlib(lua_State* L);
  void initialize_sys_mman(lua_State* L);
  void initialize_sys_socket(lua_State* L);
  void initialize_sys_stat(lua_State* L);
  void initialize_sys_statvfs(lua_State* L);
  void initialize_sys_wait(lua_State* L);
  void initialize_time(lua_State* L);
  void initialize_timer(lua_State* L);
  void initialize_timespec(lua_State* L);
  void initialize_unistd(lua_State* L);

  void initialize(lua_State* L) {
    initialize_async_service(L);
    initialize_async_task(L);
    initialize_errno(L);
    initialize_error(L);
    initialize_fcntl(L);
    initialize_fd(L);
    initialize_netdb(L);
    initialize_netinet(L);
    initialize_pathexec(L);
    initialize_pipe(L);
    initialize_process(L);
    initialize_sched(L);
    initialize_selector(L);
    initialize_selfpipe(L);
    initialize_signal(L);
    initialize_sockaddr(L);
    initialize_stdlib(L);
    initialize_sys_mman(L);
    initialize_sys_socket(L);
    initialize_sys_stat(L);
    initialize_sys_statvfs(L);
    initialize_sys_wait(L);
    initialize_time(L);
    initialize_timer(L);
    initialize_timespec(L);
    initialize_unistd(L);
  }
}

extern "C" int luaopen_dromozoa_unix(lua_State* L) {
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
