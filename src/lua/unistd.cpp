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
#include <unistd.h>

#include <vector>

#include "common.hpp"

extern char** environ;

namespace dromozoa {
  namespace {
    void impl_get_environ(lua_State* L) {
      lua_newtable(L);
      for (int i = 0; ; ++i) {
        if (const char* p = environ[i]) {
          luaX_set_field(L, -1, i + 1, p);
        } else {
          break;
        }
      }
    }

    void impl_chdir(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      if (chdir(path) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_getcwd(lua_State* L) {
      long path_max = pathconf(".", _PC_PATH_MAX);
      if (path_max == -1) {
        push_error(L);
      } else {
        std::vector<char> buffer(path_max);
        if (const char* result = getcwd(&buffer[0], buffer.size())) {
          lua_pushstring(L, result);
        } else {
          push_error(L);
        }
      }
    }

    void impl_getuid(lua_State* L) {
      luaX_push(L, getuid());
    }

    void impl_getgid(lua_State* L) {
      luaX_push(L, getgid());
    }

    void impl_geteuid(lua_State* L) {
      luaX_push(L, geteuid());
    }

    void impl_getegid(lua_State* L) {
      luaX_push(L, getegid());
    }

    void impl_getpid(lua_State* L) {
      luaX_push(L, getpid());
    }

    void impl_getpgrp(lua_State* L) {
      luaX_push(L, getpgrp());
    }

    void impl_getppid(lua_State* L) {
      luaX_push(L, getppid());
    }

    void impl_sysconf(lua_State* L) {
      int name = luaX_check_integer<int>(L, 1);
      errno = 0;
      long result = sysconf(name);
      if (errno == 0) {
        luaX_push(L, result);
      } else {
        push_error(L);
      }
    }
  }

  void initialize_unistd(lua_State* L) {
    luaX_set_field(L, -1, "get_environ", impl_get_environ);
    luaX_set_field(L, -1, "chdir", impl_chdir);
    luaX_set_field(L, -1, "getcwd", impl_getcwd);
    luaX_set_field(L, -1, "getuid", impl_getuid);
    luaX_set_field(L, -1, "getgid", impl_getgid);
    luaX_set_field(L, -1, "geteuid", impl_geteuid);
    luaX_set_field(L, -1, "getegid", impl_getegid);
    luaX_set_field(L, -1, "getpid", impl_getpid);
    luaX_set_field(L, -1, "getpgrp", impl_getpgrp);
    luaX_set_field(L, -1, "getppid", impl_getppid);
    luaX_set_field(L, -1, "sysconf", impl_sysconf);

    luaX_set_field(L, -1, "STDIN_FILENO", STDIN_FILENO);
    luaX_set_field(L, -1, "STDOUT_FILENO", STDOUT_FILENO);
    luaX_set_field(L, -1, "STDERR_FILENO", STDERR_FILENO);

    luaX_set_field<int>(L, -1, "_SC_NPROCESSORS_CONF", _SC_NPROCESSORS_CONF);
    luaX_set_field<int>(L, -1, "_SC_NPROCESSORS_ONLN", _SC_NPROCESSORS_ONLN);
  }
}
