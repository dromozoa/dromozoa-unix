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

#include <dromozoa/selector.hpp>

#if defined(HAVE_EPOLL_CREATE) || defined(HAVE_EPOLL_CREATE1)
#include <dromozoa/selector_epoll.hpp>
typedef dromozoa::selector_epoll selector_impl;
#elif defined(HAVE_KQUEUE)
#include <dromozoa/selector_kqueue.hpp>
typedef dromozoa::selector_kqueue selector_impl;
#endif

#include "common.hpp"

namespace dromozoa {
  namespace {
    selector* check_selector(lua_State* L, int arg) {
      return luaX_check_udata<selector>(L, arg, "dromozoa.unix.selector");
    }

    void impl_gc(lua_State* L) {
      check_selector(L, 1)->~selector();
    }

    void impl_call(lua_State* L) {
      size_t size = luaX_check_integer<size_t>(L, 2);
      int flags = luaX_opt_integer<int>(L, 3, SELECTOR_CLOEXEC);
      file_descriptor fd(selector_impl::open(size, flags));
      if (fd.valid()) {
        luaX_new<selector_impl>(L, fd.release(), size);
        luaX_set_metatable(L, "dromozoa.unix.selector");
      } else {
        push_error(L);
      }
    }

    void impl_close(lua_State* L) {
      if (check_selector(L, 1)->close() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_add(lua_State* L) {
      int fd = check_fd(L, 2);
      int event = luaX_check_integer<int>(L, 3);
      if (check_selector(L, 1)->add(fd, event) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_mod(lua_State* L) {
      int fd = check_fd(L, 2);
      int event = luaX_check_integer<int>(L, 3);
      if (check_selector(L, 1)->mod(fd, event) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_del(lua_State* L) {
      int fd = check_fd(L, 2);
      if (check_selector(L, 1)->del(fd) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_select(lua_State* L) {
      int result = -1;
      if (lua_isnoneornil(L, 2)) {
        result = check_selector(L, 1)->select(0);
      } else {
        struct timespec tv = {};
        check_timespec(L, 2, tv);
        result = check_selector(L, 1)->select(&tv);
      }
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, result);
      }
    }

    void impl_event(lua_State* L) {
      int i = luaX_check_integer<int>(L, 2);
      int fd = -1;
      int event = 0;
      if (check_selector(L, 1)->event(i - 1, fd, event) == -1) {
        push_error(L);
      } else {
        luaX_push(L, fd);
        luaX_push(L, event);
      }
    }
  }

  void initialize_selector(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.selector");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "close", impl_close);
      luaX_set_field(L, -1, "add", impl_add);
      luaX_set_field(L, -1, "mod", impl_mod);
      luaX_set_field(L, -1, "del", impl_del);
      luaX_set_field(L, -1, "select", impl_select);
      luaX_set_field(L, -1, "event", impl_event);
    }
    luaX_set_field(L, -2, "selector");

    luaX_set_field(L, -1, "SELECTOR_READ", SELECTOR_READ);
    luaX_set_field(L, -1, "SELECTOR_WRITE", SELECTOR_WRITE);
    luaX_set_field(L, -1, "SELECTOR_CLOEXEC", SELECTOR_CLOEXEC);
  }
}
