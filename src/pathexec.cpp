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

#include <vector>

#include <dromozoa/bind.hpp>
#include <dromozoa/pathexec.hpp>

#include "argument_vector.hpp"
#include "error.hpp"
#include "pathexec.hpp"

extern char** environ;

namespace dromozoa {
  using bind::function;
  using bind::push_success;

  namespace {
    int impl_pathexec(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      luaL_checktype(L, 2, LUA_TTABLE);
      argument_vector argv(L, 2);
      argument_vector envp(L, 3);
      std::vector<char> buffer(pathexec_buffer_size(path, argv.get()));
      if (pathexec(path, argv.get(), envp.get(), &buffer[0], buffer.size()) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }
  }

  void initialize_pathexec(lua_State* L) {
    function<impl_pathexec>::set_field(L, "pathexec");
  }
}
