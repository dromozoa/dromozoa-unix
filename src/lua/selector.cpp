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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <errno.h>
#include <time.h>

#include <iostream>
#include <new>

#include <dromozoa/bind.hpp>
#include <dromozoa/selector.hpp>

#include "common.hpp"

#if defined(HAVE_EPOLL_CREATE) || defined(HAVE_EPOLL_CREATE1)
#include <dromozoa/selector_epoll.hpp>
typedef dromozoa::selector_epoll selector_impl;
#elif defined(HAVE_KQUEUE)
#include <dromozoa/selector_kqueue.hpp>
typedef dromozoa::selector_kqueue selector_impl;
#endif

namespace dromozoa {
  using bind::function;
  using bind::get_log_level;
  using bind::push_success;

  namespace {
    selector& get_selector(lua_State* L, int n) {
      return *static_cast<selector*>(luaL_checkudata(L, n, "dromozoa.unix.selector"));
    }

    int impl_new(lua_State* L) {
      size_t size = luaL_checkinteger(L, 2);
      int flags = luaL_optinteger(L, 3, 0);
      file_descriptor fd(selector_impl::open(size, flags));
      if (!fd.valid()) {
        return push_error(L);
      }
      selector_impl* s = static_cast<selector_impl*>(lua_newuserdata(L, sizeof(selector_impl)));
      new(s) selector_impl(fd.release(), size);
      luaL_getmetatable(L, "dromozoa.unix.selector");
      lua_setmetatable(L, -2);
      if (get_log_level() > 2) {
        std::cerr << "[dromozoa-unix] new selector " << s << std::endl;
      }
      return 1;
    }

    int impl_gc(lua_State* L) {
      selector& s = get_selector(L, 1);
      s.~selector();
      return 0;
    }

    int impl_close(lua_State* L) {
      selector& s = get_selector(L, 1);
      if (s.close() == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_add(lua_State* L) {
      int fd = get_fd(L, 2);
      int event = luaL_checkinteger(L, 3);
      if (get_selector(L, 1).add(fd, event) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_mod(lua_State* L) {
      int fd = get_fd(L, 2);
      int event = luaL_checkinteger(L, 3);
      if (get_selector(L, 1).mod(fd, event) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_del(lua_State* L) {
      int fd = get_fd(L, 2);
      if (get_selector(L, 1).del(fd) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_select(lua_State* L) {
      int result = -1;
      if (lua_isnoneornil(L, 2)) {
        result = get_selector(L, 1).select(0);
      } else {
        struct timespec tv = {};
        lua_getfield(L, 2, "tv_sec");
        tv.tv_sec = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, 2, "tv_nsec");
        tv.tv_nsec = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        result = get_selector(L, 1).select(&tv);
      }
      if (result == -1) {
        int code = errno;
        if (code == EINTR) {
          return push_interrupted(L);
        } else {
          return push_error(L);
        }
      } else {
        lua_pushinteger(L, result);
        return 1;
      }
    }

    int impl_event(lua_State* L) {
      int i = luaL_checkinteger(L, 2);
      int fd = -1;
      int event = 0;
      if (get_selector(L, 1).event(i - 1, fd, event) == -1) {
        return push_error(L);
      } else {
        lua_pushinteger(L, fd);
        lua_pushinteger(L, event);
        return 2;
      }
    }
  }

  int open_selector(lua_State* L) {
    lua_newtable(L);
    function<impl_close>::set_field(L, "close");
    function<impl_add>::set_field(L, "add");
    function<impl_mod>::set_field(L, "mod");
    function<impl_del>::set_field(L, "del");
    function<impl_select>::set_field(L, "select");
    function<impl_event>::set_field(L, "event");
    lua_newtable(L);
    function<impl_new>::set_field(L, "__call");
    lua_setmetatable(L, -2);
    luaL_newmetatable(L, "dromozoa.unix.selector");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);
    return 1;
  }
}
