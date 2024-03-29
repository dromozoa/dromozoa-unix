// Copyright (C) 2016-2019,2023 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <termios.h>
#include <time.h>
#include <sys/socket.h>

#include <dromozoa/bind.hpp>

#include <dromozoa/argument_vector.hpp>
#include <dromozoa/async_service.hpp>
#include <dromozoa/socket_address.hpp>

namespace dromozoa {
  argument_vector to_argument_vector(lua_State*, int);

  class async_task : public async_service_task {
  public:
    virtual void cancel();
    virtual void result(lua_State*) = 0;
    void ref(lua_State*, int);
    void unref();
    void get_field(lua_State*);
  protected:
    async_task();
  private:
    luaX_reference<> ref_;
  };

  async_task* check_async_task(lua_State*, int);

  void push_error(lua_State*);

  void new_fd(lua_State*, int);
  int to_fd(lua_State*, int);
  int check_fd(lua_State*, int);

  void new_sockaddr(lua_State*, const socket_address&);
  void new_sockaddr(lua_State*, const struct sockaddr*, socklen_t);
  const socket_address* check_sockaddr(lua_State*, int);

  static const int TIMESPEC_TYPE_REALTIME = 0;
  static const int TIMESPEC_TYPE_MONOTONIC = 1;
  static const int TIMESPEC_TYPE_DURATION = 2;
  static const int TIMESPEC_TYPE_UNKNOWN = 3;
  void new_timespec(lua_State*, const timespec&, int);
  int check_timespec(lua_State*, int, struct timespec&, bool = true);

  void new_termios(lua_State*, const struct termios&);
  struct termios* check_termios(lua_State*, int);
}

#endif
