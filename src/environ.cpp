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

extern "C" {
#include <lua.h>
}

#include <unistd.h>

#include "environ.hpp"
#include "function.hpp"

extern char** environ;

namespace dromozoa {
  namespace {
    int impl_environ(lua_State* L) {
      lua_newtable(L);
      for (int i = 0; ; ++i) {
        if (const char* p = environ[i]) {
          lua_pushinteger(L, i + 1);
          lua_pushstring(L, p);
          lua_settable(L, -3);
        } else {
          break;
        }
      }
      return 1;
    }
  }

  void initialize_environ(lua_State* L) {
    function<impl_environ>::set_field(L, "environ");
  }
}
