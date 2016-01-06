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

#include <sys/wait.h>

#include "error.hpp"
#include "function.hpp"
#include "set_field.hpp"
#include "wait.hpp"

namespace dromozoa {
  namespace {
    int impl_wait(lua_State* L) {
      pid_t pid = luaL_optinteger(L, 1, -1);
      int status = 0;
      int options = luaL_optinteger(L, 2, 0);
      pid_t result = waitpid(pid, &status, options);
      if (result == -1) {
        return push_error(L);
      } else {
        lua_pushinteger(L, result);
        if (WIFEXITED(status)) {
          lua_pushliteral(L, "exit");
          lua_pushinteger(L, WEXITSTATUS(status));
          return 3;
        } else if (WIFSIGNALED(status)) {
          lua_pushliteral(L, "signal");
          lua_pushinteger(L, WTERMSIG(status));
          return 3;
        } else if (WIFSTOPPED(status)) {
          lua_pushliteral(L, "stop");
          lua_pushinteger(L, WSTOPSIG(status));
          return 3;
        } else if (WIFCONTINUED(status)) {
          lua_pushliteral(L, "continue");
          lua_pushinteger(L, SIGCONT);
          return 3;
        } else {
          return 1;
        }
      }
    }
  }

  void initialize_wait(lua_State* L) {
    function<impl_wait>::set_field(L, "wait");
    DROMOZOA_SET_FIELD(L, WCONTINUED);
    DROMOZOA_SET_FIELD(L, WNOHANG);
    DROMOZOA_SET_FIELD(L, WUNTRACED);
  }
}
