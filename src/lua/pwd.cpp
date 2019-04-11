// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <pwd.h>
#include <unistd.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void push_result(lua_State* L, struct passwd* result) {
      if (result) {
        lua_newtable(L);
        luaX_set_field(L, -1, "pw_name", result->pw_name);
        luaX_set_field(L, -1, "pw_uid", result->pw_uid);
        luaX_set_field(L, -1, "pw_gid", result->pw_gid);
        luaX_set_field(L, -1, "pw_dir", result->pw_dir);
        luaX_set_field(L, -1, "pw_shell", result->pw_shell);
      } else {
        luaX_push(L, luaX_nil);
      }
    }

    void impl_getpwnam(lua_State* L) {
      luaX_string_reference name = luaX_check_string(L, 1);
      long size_max = sysconf(_SC_GETPW_R_SIZE_MAX);
      if (size_max == -1) {
        push_error(L);
      } else {
        std::vector<char> buffer(size_max);
        struct passwd pwd = {};
        struct passwd* result = 0;
        if (getpwnam_r(name.data(), &pwd, &buffer[0], buffer.size(), &result) == 0) {
          push_result(L, result);
        } else {
          push_error(L);
        }
      }
    }

    void impl_getpwuid(lua_State* L) {
      uid_t uid = luaX_check_integer<uid_t>(L, 1);
      long size_max = sysconf(_SC_GETPW_R_SIZE_MAX);
      if (size_max == -1) {
        push_error(L);
      } else {
        std::vector<char> buffer(size_max);
        struct passwd pwd = {};
        struct passwd* result = 0;
        if (getpwuid_r(uid, &pwd, &buffer[0], buffer.size(), &result) == 0) {
          push_result(L, result);
        } else {
          push_error(L);
        }
      }
    }
  }

  void initialize_pwd(lua_State* L) {
    luaX_set_field(L, -1, "getpwnam", impl_getpwnam);
    luaX_set_field(L, -1, "getpwuid", impl_getpwuid);
  }
}
