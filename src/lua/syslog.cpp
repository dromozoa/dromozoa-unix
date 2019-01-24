// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <syslog.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    std::string syslog_ident;

    void impl_openlog(lua_State* L) {
      luaX_string_reference ident = luaX_to_string(L, 1);
      int option = luaX_opt_integer<int>(L, 2, 0);
      int facility = luaX_opt_integer<int>(L, 3, LOG_USER);
      if (ident) {
        syslog_ident.assign(ident.data(), ident.size());
        openlog(syslog_ident.c_str(), option, facility);
      } else {
        openlog(0, option, facility);
      }
      luaX_push_success(L);
    }

    void impl_syslog(lua_State* L) {
      int priority = luaX_check_integer<int>(L, 1);
      luaX_string_reference message = luaX_check_string(L, 2);
      syslog(priority, "%s", message.data());
      luaX_push_success(L);
    }

    void impl_closelog(lua_State* L) {
      closelog();
      syslog_ident.clear();
      luaX_push_success(L);
    }
  }

  void initialize_syslog(lua_State* L) {
    luaX_set_field(L, -1, "openlog", impl_openlog);
    luaX_set_field(L, -1, "syslog", impl_syslog);
    luaX_set_field(L, -1, "closelog", impl_closelog);

    luaX_set_field(L, -1, "LOG_PID", LOG_PID);
    luaX_set_field(L, -1, "LOG_CONS", LOG_CONS);
    luaX_set_field(L, -1, "LOG_NDELAY", LOG_NDELAY);
    luaX_set_field(L, -1, "LOG_ODELAY", LOG_ODELAY);
    luaX_set_field(L, -1, "LOG_NOWAIT", LOG_NOWAIT);

    luaX_set_field(L, -1, "LOG_KERN", LOG_KERN);
    luaX_set_field(L, -1, "LOG_USER", LOG_USER);
    luaX_set_field(L, -1, "LOG_MAIL", LOG_MAIL);
    luaX_set_field(L, -1, "LOG_NEWS", LOG_NEWS);
    luaX_set_field(L, -1, "LOG_UUCP", LOG_UUCP);
    luaX_set_field(L, -1, "LOG_DAEMON", LOG_DAEMON);
    luaX_set_field(L, -1, "LOG_AUTH", LOG_AUTH);
    luaX_set_field(L, -1, "LOG_CRON", LOG_CRON);
    luaX_set_field(L, -1, "LOG_LPR", LOG_LPR);
    luaX_set_field(L, -1, "LOG_LOCAL0", LOG_LOCAL0);
    luaX_set_field(L, -1, "LOG_LOCAL1", LOG_LOCAL1);
    luaX_set_field(L, -1, "LOG_LOCAL2", LOG_LOCAL2);
    luaX_set_field(L, -1, "LOG_LOCAL3", LOG_LOCAL3);
    luaX_set_field(L, -1, "LOG_LOCAL4", LOG_LOCAL4);
    luaX_set_field(L, -1, "LOG_LOCAL5", LOG_LOCAL5);
    luaX_set_field(L, -1, "LOG_LOCAL6", LOG_LOCAL6);
    luaX_set_field(L, -1, "LOG_LOCAL7", LOG_LOCAL7);

    luaX_set_field(L, -1, "LOG_EMERG", LOG_EMERG);
    luaX_set_field(L, -1, "LOG_ALERT", LOG_ALERT);
    luaX_set_field(L, -1, "LOG_CRIT", LOG_CRIT);
    luaX_set_field(L, -1, "LOG_ERR", LOG_ERR);
    luaX_set_field(L, -1, "LOG_WARNING", LOG_WARNING);
    luaX_set_field(L, -1, "LOG_NOTICE", LOG_NOTICE);
    luaX_set_field(L, -1, "LOG_INFO", LOG_INFO);
    luaX_set_field(L, -1, "LOG_DEBUG", LOG_DEBUG);
  }
}
