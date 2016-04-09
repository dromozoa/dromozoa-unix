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

#include <unistd.h>

#include <vector>

#include "common.hpp"

extern char** environ;

namespace dromozoa {
  namespace {
    void impl_environ(lua_State* L) {
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
    }

    void impl_getcwd(lua_State* L) {
      long path_max = pathconf(".", _PC_PATH_MAX);
      if (path_max == -1) {
        push_error(L);
      } else {
        std::vector<char> buffer(path_max);
        if (getcwd(&buffer[0], buffer.size())) {
          lua_pushstring(L, &buffer[0]);
        } else {
          push_error(L);
        }
      }
    }

    void impl_getuid(lua_State* L) {
      lua_pushinteger(L, getuid());
    }

    void impl_getgid(lua_State* L) {
      lua_pushinteger(L, getgid());
    }

    void impl_geteuid(lua_State* L) {
      lua_pushinteger(L, geteuid());
    }

    void impl_getegid(lua_State* L) {
      lua_pushinteger(L, getegid());
    }

    void impl_getpid(lua_State* L) {
      lua_pushinteger(L, getpid());
    }

    void impl_getpgrp(lua_State* L) {
      lua_pushinteger(L, getpgrp());
    }

    void impl_getppid(lua_State* L) {
      lua_pushinteger(L, getppid());
    }
  }

  void initialize_unistd(lua_State* L) {
    luaX_set_field(L, "environ", impl_environ);
    luaX_set_field(L, "getcwd", impl_getcwd);
    luaX_set_field(L, "getuid", impl_getuid);
    luaX_set_field(L, "getgid", impl_getgid);
    luaX_set_field(L, "geteuid", impl_geteuid);
    luaX_set_field(L, "getegid", impl_getegid);
    luaX_set_field(L, "getpid", impl_getpid);
    luaX_set_field(L, "getpgrp", impl_getpgrp);
    luaX_set_field(L, "getppid", impl_getppid);

    luaX_set_field(L, "STDIN_FILENO", STDIN_FILENO);
    luaX_set_field(L, "STDOUT_FILENO", STDOUT_FILENO);
    luaX_set_field(L, "STDERR_FILENO", STDERR_FILENO);
  }
}
