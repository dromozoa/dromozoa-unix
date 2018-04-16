// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <dromozoa/async_service.hpp>
#include <dromozoa/hardware_concurrency.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    async_service* check_async_service(lua_State* L, int arg) {
      return luaX_check_udata<async_service>(L, arg, "dromozoa.unix.async_service");
    }

    void impl_gc(lua_State* L) {
      check_async_service(L, 1)->~async_service();
    }

    void impl_call(lua_State* L) {
      async_service_impl* impl = 0;
      int top = lua_gettop(L);
      if (top < 2) {
        unsigned int concurrency = hardware_concurrency();
        if (concurrency < 1) {
          concurrency = 1;
        }
        impl = async_service::open(concurrency);
      } else {
        switch (top) {
          case 2:
            impl = async_service::open(
                luaX_check_integer<unsigned int>(L, 2));
            break;
          case 3:
            impl = async_service::open(
                luaX_check_integer<unsigned int>(L, 2),
                luaX_check_integer<unsigned int>(L, 3));
            break;
          default:
            impl = async_service::open(
                luaX_check_integer<unsigned int>(L, 2),
                luaX_check_integer<unsigned int>(L, 3),
                luaX_check_integer<unsigned int>(L, 4));
        }
      }
      if (impl) {
        luaX_new<async_service>(L, impl);
        luaX_set_metatable(L, "dromozoa.unix.async_service");
      } else {
        push_error(L);
      }
    }

    void impl_close(lua_State* L) {
      if (check_async_service(L, 1)->close() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_get(lua_State* L) {
      luaX_push(L, check_async_service(L, 1)->get());
    }

    void impl_read(lua_State* L) {
      luaX_push(L, check_async_service(L, 1)->read());
    }

    void impl_push(lua_State* L) {
      async_task* task = check_async_task(L, 2);
      task->ref(L, 2);
      if (check_async_service(L, 1)->push(task) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_cancel(lua_State* L) {
      async_task* task = check_async_task(L, 2);
      luaX_push(L, check_async_service(L, 1)->cancel(task));
    }

    void impl_pop(lua_State* L) {
      async_task* task = static_cast<async_task*>(check_async_service(L, 1)->pop());
      if (task) {
        task->get_field(L);
        task->unref();
      }
    }

    void impl_info(lua_State* L) {
      unsigned int spare_threads = 0;
      unsigned int current_threads = 0;
      unsigned int current_tasks = 0;
      check_async_service(L, 1)->info(spare_threads, current_threads, current_tasks);
      lua_newtable(L);
      luaX_set_field(L, -1, "spare_threads", spare_threads);
      luaX_set_field(L, -1, "current_threads", current_threads);
      luaX_set_field(L, -1, "current_tasks", current_tasks);
    }
  }

  void initialize_async_service(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.async_service");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "close", impl_close);
      luaX_set_field(L, -1, "get", impl_get);
      luaX_set_field(L, -1, "read", impl_read);
      luaX_set_field(L, -1, "push", impl_push);
      luaX_set_field(L, -1, "cancel", impl_cancel);
      luaX_set_field(L, -1, "pop", impl_pop);
      luaX_set_field(L, -1, "info", impl_info);
    }
    luaX_set_field(L, -2, "async_service");
  }
}
