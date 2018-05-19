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

#include <sys/wait.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_wait(lua_State* L) {
      pid_t pid = luaX_opt_integer<pid_t>(L, 1, -1);
      int options = luaX_opt_integer<int>(L, 2, 0);
      int status = 0;
      pid_t result = waitpid(pid, &status, options);
      if (result == -1) {
        push_error(L);
      } else {
        luaX_push(L, result);
        if (result != 0) {
          if (WIFEXITED(status)) {
            luaX_push(L, "exit", WEXITSTATUS(status));
          } else if (WIFSIGNALED(status)) {
            luaX_push(L, "signal", WTERMSIG(status));
          } else if (WIFSTOPPED(status)) {
            luaX_push(L, "stop", WSTOPSIG(status));
          } else if (WIFCONTINUED(status)) {
            luaX_push(L, "continue", SIGCONT);
          }
        }
      }
    }
  }

  void initialize_sys_wait(lua_State* L) {
    luaX_set_field(L, -1, "wait", impl_wait);

    luaX_set_field(L, -1, "WCONTINUED", WCONTINUED);
    luaX_set_field(L, -1, "WNOHANG", WNOHANG);
    luaX_set_field(L, -1, "WUNTRACED", WUNTRACED);
  }
}
