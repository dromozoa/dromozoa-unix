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
#include <lauxlib.h>
}

#include <limits.h>
#include <stdlib.h>

#include <vector>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "fd.hpp"
#include "stdlib.hpp"

namespace dromozoa {
  using bind::function;

  namespace {
    int impl_realpath(lua_State* L) {
#ifdef PATH_MAX
      std::vector<char> buffer(PATH_MAX);
      if (const char* result = realpath(luaL_checkstring(L, 1), &buffer[0])) {
        lua_pushstring(L, result);
        return 1;
      }
#else
      if (char* result = realpath(luaL_checkstring(L, 1), 0)) {
        lua_pushstring(L, result);
        free(result);
        return 1;
      }
#endif
      else {
        return push_error(L);
      }
    }

    int impl_mkdtemp(lua_State* L) {
      size_t size = 0;
      const char* tmpl = luaL_checklstring(L, 1, &size);
      std::vector<char> buffer(tmpl, tmpl + size + 1);
      if (const char* result = mkdtemp(&buffer[0])) {
        lua_pushstring(L, result);
        return 1;
      } else {
        return push_error(L);
      }
    }

    int impl_mkstemp(lua_State* L) {
      size_t size = 0;
      const char* tmpl = luaL_checklstring(L, 1, &size);
      std::vector<char> buffer(tmpl, tmpl + size + 1);
      int result = mkstemp(&buffer[0]);
      if (result == -1) {
        return push_error(L);
      } else {
        new_fd(L, result);
        lua_pushstring(L, &buffer[0]);
        return 2;
      }
    }
  }

  void initialize_stdlib(lua_State* L) {
    function<impl_realpath>::set_field(L, "realpath");
    function<impl_mkdtemp>::set_field(L, "mkdtemp");
    function<impl_mkstemp>::set_field(L, "mkstemp");
  }
}
