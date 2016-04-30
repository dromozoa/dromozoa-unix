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

#include <errno.h>

#include <dromozoa/forkexec.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_call(lua_State* L) {
      lua_newtable(L);
      luaX_set_metatable(L, "dromozoa.unix.process");
    }

    void impl_forkexec(lua_State* L) {
      const char* path = luaL_checkstring(L, 2);
      luaL_checktype(L, 3, LUA_TTABLE);
      argument_vector argv = to_argument_vector(L, 3);
      argument_vector envp = to_argument_vector(L, 4);
      const char* chdir = lua_tostring(L, 5);
      int dup2_stdio[3] = { -1, -1, -1 };
      if (lua_istable(L, 6)) {
        for (int i = 0; i < 3; ++i) {
          luaX_get_field(L, 6, i);
          dup2_stdio[i] = to_fd(L, -1);
          lua_pop(L, 1);
        }
      }

      pid_t pid = -1;
      int result = forkexec(path, argv.get(), envp.get(), chdir, dup2_stdio, pid);
      int code = errno;

      if (pid != -1) {
        luaX_set_field(L, 1, 1, pid);
      }
      if (result == -1) {
        push_error(L, code);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_forkexec_daemon(lua_State* L) {
      const char* path = luaL_checkstring(L, 2);
      luaL_checktype(L, 3, LUA_TTABLE);
      argument_vector argv = to_argument_vector(L, 3);
      argument_vector envp = to_argument_vector(L, 4);
      const char* chdir = lua_tostring(L, 5);

      pid_t pid1 = -1;
      pid_t pid2 = -1;
      int result = forkexec_daemon(path, argv.get(), envp.get(), chdir, pid1, pid2);
      int code = errno;

      if (pid1 != -1) {
        luaX_set_field(L, 1, 1, pid1);
      }
      if (pid2 != -1) {
        luaX_set_field(L, 1, 2, pid2);
      }
      if (result == -1) {
        push_error(L, code);
      } else {
        luaX_push_success(L);
      }
    }
  }

  void initialize_process(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.process");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "forkexec", impl_forkexec);
      luaX_set_field(L, -1, "forkexec_daemon", impl_forkexec_daemon);
    }
    luaX_set_field(L, -2, "process");
  }
}
