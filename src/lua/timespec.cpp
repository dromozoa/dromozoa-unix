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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include <sys/time.h>

#include <iomanip>
#include <sstream>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_add(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      check_timespec(L, 1, tv1);
      check_timespec(L, 2, tv2);
      tv1.tv_sec += tv2.tv_sec;
      tv1.tv_nsec += tv2.tv_nsec;
      if (tv1.tv_nsec > 999999999) {
        ++tv1.tv_sec;
        tv1.tv_nsec -= 1000000000;
      }
      new_timespec(L, tv1);
    }

    void impl_sub(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      check_timespec(L, 1, tv1);
      check_timespec(L, 2, tv2);
      tv1.tv_sec -= tv2.tv_sec;
      tv1.tv_nsec -= tv2.tv_nsec;
      if (tv1.tv_nsec < 0) {
        --tv1.tv_sec;
        tv1.tv_nsec += 1000000000;
      }
      new_timespec(L, tv1);
    }

    void impl_eq(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      check_timespec(L, 1, tv1);
      check_timespec(L, 2, tv2);
      luaX_push(L, tv1.tv_sec == tv2.tv_sec && tv1.tv_nsec == tv2.tv_nsec);
    }

    void impl_lt(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      check_timespec(L, 1, tv1);
      check_timespec(L, 2, tv2);
      if (tv1.tv_sec == tv2.tv_sec) {
        return luaX_push(L, tv1.tv_nsec < tv2.tv_nsec);
      } else {
        return luaX_push(L, tv1.tv_sec < tv2.tv_sec);
      }
    }

    void impl_le(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      check_timespec(L, 1, tv1);
      check_timespec(L, 2, tv2);
      if (tv1.tv_sec == tv2.tv_sec) {
        return luaX_push(L, tv1.tv_nsec <= tv2.tv_nsec);
      } else {
        return luaX_push(L, tv1.tv_sec <= tv2.tv_sec);
      }
    }

    void impl_call(lua_State* L) {
      struct timespec tv = {};
      check_timespec(L, 2, tv);
      new_timespec(L, tv);
    }

#ifdef HAVE_CLOCK_GETTIME
    void impl_now(lua_State* L) {
      struct timespec tv = {};
      if (clock_gettime(CLOCK_REALTIME, &tv) == -1) {
        push_error(L);
      } else {
        new_timespec(L, tv);
      }
    }
#else
    void impl_now(lua_State* L) {
      struct timeval tv = {};
      if (gettimeofday(&tv, 0) == -1) {
        push_error(L);
      } else {
        lua_newtable(L);
        luaX_set_field(L, -1, "tv_sec", tv.tv_sec);
        luaX_set_field(L, -1, "tv_nsec", tv.tv_usec * 1000);
        luaX_set_metatable(L, "dromozoa.unix.timespec");
      }
    }
#endif

    void impl_tostring(lua_State* L) {
      struct timespec tv = {};
      check_timespec(L, 1, tv);
      bool utc = lua_toboolean(L, 2);
      struct tm tm = {};
      if (utc) {
        if (!gmtime_r(&tv.tv_sec, &tm)) {
          push_error(L);
          return;
        }
      } else {
        if (!localtime_r(&tv.tv_sec, &tm)) {
          push_error(L);
          return;
        }
      }
      std::ostringstream out;
      out << std::setfill('0')
          << (tm.tm_year + 1900)
          << "-" << std::setw(2) << (tm.tm_mon + 1)
          << "-" << std::setw(2) << tm.tm_mday
          << "T" << std::setw(2) << tm.tm_hour
          << ":" << std::setw(2) << tm.tm_min
          << ":" << std::setw(2) << tm.tm_sec
          << "." << std::setw(9) << tv.tv_nsec;
      if (utc) {
        out << "Z";
      } else {
        char buffer[6] = { 0 };
        strftime(buffer, 6, "%z", &tm);
        out << buffer;
      }
      std::string s = out.str();
      lua_pushlstring(L, s.data(), s.size());
    }

    void impl_tonumber(lua_State* L) {
      struct timespec tv = {};
      check_timespec(L, 1, tv);
      lua_pushnumber(L, tv.tv_sec + tv.tv_nsec * 0.000000001);
    }
  }

  void new_timespec(lua_State* L, const timespec& tv) {
    lua_newtable(L);
    luaX_set_field(L, -1, "tv_sec", tv.tv_sec);
    luaX_set_field(L, -1, "tv_nsec", tv.tv_nsec);
    luaX_set_metatable(L, "dromozoa.unix.timespec");
  }

  bool check_timespec(lua_State* L, int arg, struct timespec& tv) {
    if (lua_isnoneornil(L, arg)) {
      return false;
    } else if (lua_isnumber(L, arg)) {
      double t = lua_tonumber(L, arg);
      double i = 0;
      double f = modf(t, &i);
      tv.tv_sec = i;
      tv.tv_nsec = f * 1000000000;
      return true;
    } else if (lua_istable(L, arg)) {
      tv.tv_sec = luaX_opt_integer_field<time_t>(L, arg, "tv_sec", 0);
      tv.tv_nsec = luaX_opt_integer_field<long>(L, arg, "tv_nsec", 0, 0L, 999999999L);
      return true;
    } else {
      luaL_argerror(L, arg, "nil, number or table expected");
      return false;
    }
  }

  void initialize_timespec(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.timespec");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__add", impl_add);
      luaX_set_field(L, -1, "__sub", impl_sub);
      luaX_set_field(L, -1, "__eq", impl_eq);
      luaX_set_field(L, -1, "__lt", impl_lt);
      luaX_set_field(L, -1, "__le", impl_le);
      luaX_set_field(L, -1, "__tostring", impl_tostring);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "now", impl_now);
      luaX_set_field(L, -1, "tostring", impl_tostring);
      luaX_set_field(L, -1, "tonumber", impl_tonumber);
    }
    luaX_set_field(L, -2, "timespec");
  }
}
