// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <math.h>

#include <iomanip>
#include <sstream>

#include "common.hpp"

namespace dromozoa {
  namespace {
    static const int TIMESPEC_TYPE_MIN = TIMESPEC_TYPE_REALTIME;
    static const int TIMESPEC_TYPE_MAX = TIMESPEC_TYPE_UNKNOWN;
    static const int TIMESPEC_TYPE_SIZE = TIMESPEC_TYPE_UNKNOWN + 1;

    static const int R = TIMESPEC_TYPE_REALTIME;
    static const int M = TIMESPEC_TYPE_MONOTONIC;
    static const int D = TIMESPEC_TYPE_DURATION;
    static const int U = TIMESPEC_TYPE_UNKNOWN;

    static const int TIMESPEC_TYPE_MATRIX_ADD[] = {
      U, U, R, R,
      U, U, M, M,
      R, M, D, U,
      R, M, U, U,
    };

    static const int TIMESPEC_TYPE_MATRIX_SUB[] = {
      D, U, R, U,
      U, D, M, U,
      U, U, D, D,
      D, D, U, U,
    };

    static const int TIMESPEC_TYPE_MATRIX_CALL[] = {
      R, M, D, R,
      R, M, D, M,
      R, M, D, D,
      R, M, D, U,
    };

    void impl_add(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      int type1 = check_timespec(L, 1, tv1);
      int type2 = check_timespec(L, 2, tv2);
      tv1.tv_sec += tv2.tv_sec;
      tv1.tv_nsec += tv2.tv_nsec;
      if (tv1.tv_nsec > 999999999) {
        ++tv1.tv_sec;
        tv1.tv_nsec -= 1000000000;
      }
      new_timespec(L, tv1, TIMESPEC_TYPE_MATRIX_ADD[type1 * TIMESPEC_TYPE_SIZE + type2]);
    }

    void impl_sub(lua_State* L) {
      struct timespec tv1 = {};
      struct timespec tv2 = {};
      int type1 = check_timespec(L, 1, tv1);
      int type2 = check_timespec(L, 2, tv2);
      tv1.tv_sec -= tv2.tv_sec;
      tv1.tv_nsec -= tv2.tv_nsec;
      if (tv1.tv_nsec < 0) {
        --tv1.tv_sec;
        tv1.tv_nsec += 1000000000;
      }
      new_timespec(L, tv1, TIMESPEC_TYPE_MATRIX_SUB[type1 * TIMESPEC_TYPE_SIZE + type2]);
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
      int type1 = check_timespec(L, 2, tv);
      int type2 = luaX_opt_integer<int>(L, 3, TIMESPEC_TYPE_UNKNOWN, TIMESPEC_TYPE_MIN, TIMESPEC_TYPE_MAX);
      new_timespec(L, tv, TIMESPEC_TYPE_MATRIX_CALL[type1 * TIMESPEC_TYPE_SIZE + type2]);
    }

    void impl_tostring(lua_State* L) {
      struct timespec tv = {};
      int type = check_timespec(L, 1, tv);
      std::ostringstream out;
      out << std::setfill('0');
      if (type == TIMESPEC_TYPE_REALTIME) {
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
        out << (tm.tm_year + 1900)
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
      } else {
        out << tv.tv_sec << "." << std::setw(9) << tv.tv_nsec;
      }
      luaX_push(L, out.str());
    }

    void impl_tonumber(lua_State* L) {
      struct timespec tv = {};
      check_timespec(L, 1, tv);
      luaX_push(L, tv.tv_sec + tv.tv_nsec * 0.000000001);
    }
  }

  void new_timespec(lua_State* L, const timespec& tv, int type) {
    lua_newtable(L);
    luaX_set_field(L, -1, "tv_sec", tv.tv_sec);
    luaX_set_field(L, -1, "tv_nsec", tv.tv_nsec);
    luaX_set_field(L, -1, "tv_type", type);
    luaX_set_metatable(L, "dromozoa.unix.timespec");
  }

  int check_timespec(lua_State* L, int arg, struct timespec& tv) {
    if (lua_isnumber(L, arg)) {
      double t = lua_tonumber(L, arg);
      double i = 0;
      double f = modf(t, &i);
      tv.tv_sec = i;
      tv.tv_nsec = f * 1000000000;
      return TIMESPEC_TYPE_UNKNOWN;
    } else if (lua_istable(L, arg)) {
      tv.tv_sec = luaX_opt_integer_field<time_t>(L, arg, "tv_sec", 0);
      tv.tv_nsec = luaX_opt_integer_field<long>(L, arg, "tv_nsec", 0, 0L, 999999999L);
      return luaX_opt_integer_field<int>(L, arg, "tv_type", TIMESPEC_TYPE_UNKNOWN, TIMESPEC_TYPE_MIN, TIMESPEC_TYPE_MAX);
    } else {
      luaL_argerror(L, arg, "number or table expected");
      return TIMESPEC_TYPE_UNKNOWN;
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
      luaX_set_field(L, -1, "add", impl_add);
      luaX_set_field(L, -1, "sub", impl_sub);
      luaX_set_field(L, -1, "eq", impl_eq);
      luaX_set_field(L, -1, "lt", impl_lt);
      luaX_set_field(L, -1, "le", impl_le);
      luaX_set_field(L, -1, "tostring", impl_tostring);
      luaX_set_field(L, -1, "tonumber", impl_tonumber);
    }
    luaX_set_field(L, -2, "timespec");

    luaX_set_field(L, -1, "TIMESPEC_TYPE_REALTIME", TIMESPEC_TYPE_REALTIME);
    luaX_set_field(L, -1, "TIMESPEC_TYPE_MONOTONIC", TIMESPEC_TYPE_MONOTONIC);
    luaX_set_field(L, -1, "TIMESPEC_TYPE_DURATION", TIMESPEC_TYPE_DURATION);
    luaX_set_field(L, -1, "TIMESPEC_TYPE_UNKNOWN", TIMESPEC_TYPE_UNKNOWN);
  }
}
