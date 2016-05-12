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

#include <sched.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_sched_yield(lua_State* L) {
      if (sched_yield() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_sched_get_priority_min(lua_State* L) {
      int policy = luaX_check_integer<int>(L, 1);
      int result = sched_get_priority_min(policy);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, result);
      }
    }

    void impl_sched_get_priority_max(lua_State* L) {
      int policy = luaX_check_integer<int>(L, 1);
      int result = sched_get_priority_max(policy);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, result);
      }
    }

#ifdef HAVE_SCHED_GETSCHEDULER
    void impl_sched_getscheduler(lua_State* L) {
      pid_t pid = luaX_check_integer<pid_t>(L, 1);
      int result = sched_getscheduler(pid);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, result);
      }
    }
#endif

#ifdef HAVE_SCHED_GETPARAM
    void impl_sched_getparam(lua_State* L) {
      pid_t pid = luaX_check_integer<pid_t>(L, 1);
      struct sched_param param = {};
      if (sched_getparam(pid, &param) == -1) {
        push_error(L);
      } else {
        lua_newtable(L);
        luaX_set_field(L, -1, "sched_priority", param.sched_priority);
      }
    }
#endif

#ifdef HAVE_SCHED_GETAFFINITY
    void impl_sched_getaffinity(lua_State* L) {
      pid_t pid = luaX_check_integer<pid_t>(L, 1);
      cpu_set_t mask;
      CPU_ZERO(&mask);
      if (sched_getaffinity(pid, sizeof(mask), &mask) == -1) {
        push_error(L);
      } else {
        lua_newtable(L);
        int i = 0;
        int n = CPU_COUNT(&mask);
        for (int cpu = 0; i < n; ++cpu) {
          if (CPU_ISSET(cpu, &mask)) {
            ++i;
            luaX_set_field(L, -1, i, cpu);
          }
        }
      }
    }
#endif

#ifdef HAVE_SCHED_SETSCHEDULER
    void impl_sched_setscheduler(lua_State* L) {
      pid_t pid = luaX_check_integer<pid_t>(L, 1);
      int policy = luaX_check_integer<int>(L, 2);
      struct sched_param param = {};
      param.sched_priority = luaX_check_integer_field<int>(L, 3, "sched_priority");
      if (sched_setscheduler(pid, policy, &param) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
#endif

#ifdef HAVE_SCHED_SETAFFINITY
    void impl_sched_setaffinity(lua_State* L) {
      pid_t pid = luaX_check_integer<pid_t>(L, 1);
      cpu_set_t mask;
      CPU_ZERO(&mask);
      if (lua_istable(L, 2)) {
        for (int i = 1; ; ++i) {
          luaX_get_field(L, 2, i);
          if (lua_isnumber(L, -1)) {
            int cpu = lua_tointeger(L, -1);
            CPU_SET(cpu, &mask);
            lua_pop(L, 1);
          } else {
            lua_pop(L, 1);
            break;
          }
        }
      }
      if (sched_setaffinity(pid, sizeof(mask), &mask) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
#endif
  }

  void initialize_sched(lua_State* L) {
    luaX_set_field(L, -1, "sched_yield", impl_sched_yield);
    luaX_set_field(L, -1, "sched_get_priority_min", impl_sched_get_priority_min);
    luaX_set_field(L, -1, "sched_get_priority_max", impl_sched_get_priority_max);
#ifdef HAVE_SCHED_GETSCHEDULER
    luaX_set_field(L, -1, "sched_getscheduler", impl_sched_getscheduler);
#endif
#ifdef HAVE_SCHED_GETPARAM
    luaX_set_field(L, -1, "sched_getparam", impl_sched_getparam);
#endif
#ifdef HAVE_SCHED_GETAFFINITY
    luaX_set_field(L, -1, "sched_getaffinity", impl_sched_getaffinity);
#endif
#ifdef HAVE_SCHED_SETSCHEDULER
    luaX_set_field(L, -1, "sched_setscheduler", impl_sched_setscheduler);
#endif
#ifdef HAVE_SCHED_SETAFFINITY
    luaX_set_field(L, -1, "sched_setaffinity", impl_sched_setaffinity);
#endif

#ifdef SCHED_BATCH
    luaX_set_field(L, -1, "SCHED_BATCH", SCHED_BATCH);
#endif
    luaX_set_field(L, -1, "SCHED_FIFO", SCHED_FIFO);
#ifdef SCHED_IDLE
    luaX_set_field(L, -1, "SCHED_IDLE", SCHED_IDLE);
#endif
    luaX_set_field(L, -1, "SCHED_OTHER", SCHED_OTHER);
    luaX_set_field(L, -1, "SCHED_RR", SCHED_RR);
#ifdef SCHED_SPORADIC
    luaX_set_field(L, -1, "SCHED_SPORADIC", SCHED_SPORADIC);
#endif
  }
}
