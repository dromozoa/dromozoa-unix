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

#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>

#include <vector>

#include <dromozoa/compat_mkostemp.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_realpath(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
#ifdef PATH_MAX
      std::vector<char> buffer(PATH_MAX);
      if (const char* result = realpath(path, &buffer[0])) {
        luaX_push(L, result);
      }
#else
      if (char* result = realpath(path, 0)) {
        luaX_push(L, result);
        free(result);
      }
#endif
      else {
        push_error(L);
      }
    }

    void impl_mkdtemp(lua_State* L) {
      size_t size = 0;
      const char* tmpl = luaL_checklstring(L, 1, &size);
      std::vector<char> buffer(tmpl, tmpl + size + 1);
      if (const char* result = mkdtemp(&buffer[0])) {
        luaX_push(L, result);
      } else {
        push_error(L);
      }
    }

    void impl_mkstemp(lua_State* L) {
      size_t size = 0;
      const char* tmpl = luaL_checklstring(L, 1, &size);
      int flags = luaX_opt_integer<int>(L, 2, O_CLOEXEC);
      std::vector<char> buffer(tmpl, tmpl + size + 1);
      int result = compat_mkostemp(&buffer[0], flags);
      if (result == -1) {
        push_error(L);
      } else {
        new_fd(L, result);
        luaX_push(L, &buffer[0]);
      }
    }
  }

  void initialize_stdlib(lua_State* L) {
    luaX_set_field(L, -1, "realpath", impl_realpath);
    luaX_set_field(L, -1, "mkdtemp", impl_mkdtemp);
    luaX_set_field(L, -1, "mkstemp", impl_mkstemp);
  }
}
