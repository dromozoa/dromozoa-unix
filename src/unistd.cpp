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

#include <vector>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "unistd.hpp"

extern char** environ;

namespace dromozoa {
  using bind::function;

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

    int impl_getcwd(lua_State* L) {
      long path_max = pathconf(".", _PC_PATH_MAX);
      if (path_max == -1) {
        return push_error(L);
      } else {
        std::vector<char> buffer(path_max);
        if (getcwd(&buffer[0], buffer.size())) {
          lua_pushstring(L, &buffer[0]);
          return 1;
        } else {
          return push_error(L);
        }
      }
    }

    int impl_getuid(lua_State* L) {
      lua_pushinteger(L, getuid());
      return 1;
    }

    int impl_getgid(lua_State* L) {
      lua_pushinteger(L, getgid());
      return 1;
    }

    int impl_geteuid(lua_State* L) {
      lua_pushinteger(L, geteuid());
      return 1;
    }

    int impl_getegid(lua_State* L) {
      lua_pushinteger(L, getegid());
      return 1;
    }

    int impl_getpid(lua_State* L) {
      lua_pushinteger(L, getpid());
      return 1;
    }

    int impl_getpgrp(lua_State* L) {
      lua_pushinteger(L, getpgrp());
      return 1;
    }

    int impl_getppid(lua_State* L) {
      lua_pushinteger(L, getppid());
      return 1;
    }
  }

  void initialize_unistd(lua_State* L) {
    function<impl_environ>::set_field(L, "environ");
    function<impl_getcwd>::set_field(L, "getcwd");
    function<impl_getuid>::set_field(L, "getuid");
    function<impl_getgid>::set_field(L, "getgid");
    function<impl_geteuid>::set_field(L, "geteuid");
    function<impl_getegid>::set_field(L, "getegid");
    function<impl_getpid>::set_field(L, "getpid");
    function<impl_getpgrp>::set_field(L, "getpgrp");
    function<impl_getppid>::set_field(L, "getppid");

    DROMOZOA_BIND_SET_FIELD(L, STDIN_FILENO);
    DROMOZOA_BIND_SET_FIELD(L, STDOUT_FILENO);
    DROMOZOA_BIND_SET_FIELD(L, STDERR_FILENO);
  }
}
