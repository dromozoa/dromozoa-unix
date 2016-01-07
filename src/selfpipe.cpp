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

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "error.hpp"
#include "function.hpp"
#include "pipe.hpp"
#include "selfpipe.hpp"
#include "success.hpp"

static int dromozoa_selfpipe_fd[2] = { -1, -1 };

extern "C" void dromozoa_selfpipe_catch(int) {
  write(dromozoa_selfpipe_fd[1], "", 1);
}

namespace dromozoa {
  namespace {
    int impl_install(lua_State* L) {
      if (dromozoa_selfpipe_fd[0] == -1) {
        if (pipe2(dromozoa_selfpipe_fd, O_CLOEXEC | O_NONBLOCK) == -1) {
          return push_error(L);
        }
      }
      struct sigaction sa = {};
      sa.sa_handler = dromozoa_selfpipe_catch;
      if (sigaction(SIGCHLD, &sa, 0) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_uninstall(lua_State* L) {
      close_pipe(dromozoa_selfpipe_fd);
      struct sigaction sa = {};
      sa.sa_handler = SIG_DFL;
      if (sigaction(SIGCHLD, &sa, 0) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_get(lua_State* L) {
      lua_pushinteger(L, dromozoa_selfpipe_fd[0]);
      return 1;
    }

    int impl_read(lua_State* L) {
      int count = 0;
      char c;
      while (read(dromozoa_selfpipe_fd[0], &c, 1) == 1) {
        ++count;
      }
      lua_pushinteger(L, count);
      return 1;
    }
  }

  int open_selfpipe(lua_State* L) {
    lua_newtable(L);
    function<impl_install>::set_field(L, "install");
    function<impl_uninstall>::set_field(L, "uninstall");
    function<impl_get>::set_field(L, "get");
    function<impl_read>::set_field(L, "read");
    return 1;
  }
}
