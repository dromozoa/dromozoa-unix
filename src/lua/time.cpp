// Copyright (C) 2015,2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <errno.h>

#include <dromozoa/async_task.hpp>
#include <dromozoa/compat_clock_gettime.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_nanosleep(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      check_timespec(L, 1, tv1);
      if (nanosleep(&tv1, &tv2) == -1) {
        push_error(L);
        new_timespec(L, tv2, TIMESPEC_TYPE_DURATION);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_clock_gettime(lua_State* L) {
      int clock_id = luaX_check_integer<int>(L, 1);
      struct timespec tv = {};
      if (compat_clock_gettime(clock_id, &tv) == -1) {
        push_error(L);
      } else {
        if (clock_id == COMPAT_CLOCK_REALTIME) {
          new_timespec(L, tv, TIMESPEC_TYPE_REALTIME);
        } else {
          new_timespec(L, tv, TIMESPEC_TYPE_MONOTONIC);
        }
      }
    }

    class async_nanosleep : public async_task {
    public:
      explicit async_nanosleep(lua_State* L, const struct timespec& tv1) : L_(L), tv1_(tv1), tv2_(), result_(), code_() {}

      virtual void dispatch() {
        result_ = nanosleep(&tv1_, &tv2_);
        if (result_ == -1) {
          code_ = errno;
        }
      }

      virtual void cancel() {
        unref_async_task(L_, this);
      }

      virtual void result(void* state) {
        lua_State* L = static_cast<lua_State*>(state);
        if (result_ == -1) {
          errno = code_;
          push_error(L);
          new_timespec(L, tv2_, TIMESPEC_TYPE_DURATION);
        } else {
          luaX_push_success(L);
        }
      }

    private:
      lua_State* L_;
      struct timespec tv1_;
      struct timespec tv2_;
      int result_;
      int code_;
      async_nanosleep(const async_nanosleep&);
      async_nanosleep& operator=(const async_nanosleep&);
    };

    void impl_async_nanosleep(lua_State* L) {
      struct timespec tv1 = {};
      check_timespec(L, 1, tv1);
      luaX_new<async_nanosleep>(L, L, tv1);
      luaX_set_metatable(L, "dromozoa.unix.async_task");
    }
  }

  void initialize_time(lua_State* L) {
    luaX_set_field(L, -1, "nanosleep", impl_nanosleep);
    luaX_set_field(L, -1, "clock_gettime", impl_clock_gettime);
    luaX_set_field(L, -1, "async_nanosleep", impl_async_nanosleep);

    luaX_set_field(L, -1, "CLOCK_REALTIME", COMPAT_CLOCK_REALTIME);
    luaX_set_field(L, -1, "CLOCK_MONOTONIC", COMPAT_CLOCK_MONOTONIC);
    luaX_set_field(L, -1, "CLOCK_MONOTONIC_RAW", COMPAT_CLOCK_MONOTONIC_RAW);
  }
}
