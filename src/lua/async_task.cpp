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

#include "common.hpp"

namespace dromozoa {
  async_task_impl::async_task_impl() : ref_() {}

  async_task_impl::~async_task_impl() {
    unref();
  }

  void async_task_impl::cancel() {
    unref();
  }

  void async_task_impl::result(void* state) {
    impl_result(static_cast<lua_State*>(state));
  }

  void async_task_impl::ref(lua_State* L, int index) {
    unref();
    ref_ = new luaX_reference<1>(L, index);
  }

  void async_task_impl::get_field() {
    ref_->get_field();
  }

  void async_task_impl::unref() {
    if (ref_) {
      delete ref_;
      ref_ = 0;
    }
  }

  namespace {
    void impl_gc(lua_State* L) {
      check_async_task(L, 1)->~async_task();
    }

    void impl_result(lua_State* L) {
      check_async_task(L, 1)->result(L);
    }
  }

  async_task* check_async_task(lua_State* L, int arg) {
    return luaX_check_udata<async_task>(L, arg, "dromozoa.unix.async_task");
  }

  void initialize_async_task(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.async_task");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "result", impl_result);
    }
    luaX_set_field(L, -2, "async_task");
  }
}
