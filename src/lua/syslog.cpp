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
    void impl_openlog(lua_State* L) {
    }

    void impl_syslog(lua_State* L) {
    }

    void impl_closelog(lua_State* L) {
    }
  }

  void initialize_sys_socket(lua_State* L) {
    luaX_set_field(L, -1, "openlog", impl_openlog);
    luaX_set_field(L, -1, "syslog", impl_syslog);
    luaX_set_field(L, -1, "closelog", impl_closelog);
  }
}
