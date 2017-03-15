// Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <time.h>

#include <dromozoa/bind.hpp>

#include <dromozoa/argument_vector.hpp>
#include <dromozoa/async_task.hpp>
#include <dromozoa/socket_address.hpp>

namespace dromozoa {
  argument_vector to_argument_vector(lua_State* L, int arg);

  void push_error(lua_State* L);

  class async_task_impl : public async_task {
  public:
    virtual void cancel();
    virtual void result(void* state);
    void ref(lua_State* L, int index);
    void unref(bool get_field);
  protected:
    async_task_impl();
    virtual void impl_result(lua_State* L) = 0;
  private:
    luaX_reference<> ref_;
  };

  async_task* check_async_task(lua_State* L, int arg);

  void new_fd(lua_State* L, int fd);
  int to_fd(lua_State* L, int index);
  int check_fd(lua_State* L, int arg);

  void new_sockaddr(lua_State* L, const socket_address& address);
  void new_sockaddr(lua_State* L, const struct sockaddr* address, socklen_t size);
  const socket_address* check_sockaddr(lua_State* L, int arg);

  static const int TIMESPEC_TYPE_REALTIME = 0;
  static const int TIMESPEC_TYPE_MONOTONIC = 1;
  static const int TIMESPEC_TYPE_DURATION = 2;
  static const int TIMESPEC_TYPE_UNKNOWN = 3;
  void new_timespec(lua_State* L, const timespec& tv, int type);
  int check_timespec(lua_State* L, int arg, struct timespec& tv);
}

#endif
