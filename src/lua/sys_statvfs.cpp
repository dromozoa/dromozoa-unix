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

#include <sys/statvfs.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void new_statvfs(lua_State* L, const struct statvfs& buf) {
      lua_newtable(L);
      luaX_set_field(L, -1, "f_bsize", buf.f_bsize);
      luaX_set_field(L, -1, "f_frsize", buf.f_frsize);
      luaX_set_field(L, -1, "f_blocks", buf.f_blocks);
      luaX_set_field(L, -1, "f_bfree", buf.f_bfree);
      luaX_set_field(L, -1, "f_bavail", buf.f_bavail);
      luaX_set_field(L, -1, "f_files", buf.f_files);
      luaX_set_field(L, -1, "f_ffree", buf.f_ffree);
      luaX_set_field(L, -1, "f_favail", buf.f_favail);
      luaX_set_field(L, -1, "f_fsid", buf.f_fsid);
      luaX_set_field(L, -1, "f_flag", buf.f_flag);
      luaX_set_field(L, -1, "f_namemax", buf.f_namemax);
    }

    void impl_statvfs(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      struct statvfs buf = {};
      if (statvfs(path, &buf) == -1) {
        push_error(L);
      } else {
        new_statvfs(L, buf);
      }
    }

    void impl_fstatvfs(lua_State* L) {
      struct statvfs buf = {};
      if (fstatvfs(check_fd(L, 1), &buf) == -1) {
        push_error(L);
      } else {
        new_statvfs(L, buf);
      }
    }
  }

  void initialize_sys_statvfs(lua_State* L) {
    luaX_set_field(L, -1, "statvfs", impl_statvfs);

    luaX_set_field<unsigned long>(L, -1, "ST_RDONLY", ST_RDONLY);
    luaX_set_field<unsigned long>(L, -1, "ST_NOSUID", ST_NOSUID);
  }

  void initialize_fd_statvfs(lua_State* L) {
    luaX_set_field(L, -1, "fstatvfs", impl_fstatvfs);
  }
}
