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

#include <sys/stat.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void new_stat(lua_State* L, const struct stat& buf) {
      lua_newtable(L);
      luaX_set_field(L, -1, "st_dev", buf.st_dev);
      luaX_set_field(L, -1, "st_ino", buf.st_ino);
      luaX_set_field(L, -1, "st_mode", buf.st_mode);
      luaX_set_field(L, -1, "st_nlink", buf.st_nlink);
      luaX_set_field(L, -1, "st_uid", buf.st_uid);
      luaX_set_field(L, -1, "st_gid", buf.st_gid);
      luaX_set_field(L, -1, "st_rdev", buf.st_rdev);
      luaX_set_field(L, -1, "st_size", buf.st_size);
      luaX_set_field(L, -1, "st_atime", buf.st_atime);
      luaX_set_field(L, -1, "st_ctime", buf.st_ctime);
      luaX_set_field(L, -1, "st_mtime", buf.st_mtime);
      // new_timespec(L, buf.st_atim, TIMESPEC_TYPE_REALTIME);
      // luaX_set_field(L, -2, "st_atim");
      // new_timespec(L, buf.st_mtim, TIMESPEC_TYPE_REALTIME);
      // luaX_set_field(L, -2, "st_mtim");
      // new_timespec(L, buf.st_ctim, TIMESPEC_TYPE_REALTIME);
      // luaX_set_field(L, -2, "st_ctim");
      luaX_set_field(L, -1, "st_blksize", buf.st_blksize);
      luaX_set_field(L, -1, "st_blocks", buf.st_blocks);
    }

    void impl_stat(lua_State* L) {
      const char* path = luaL_checkstring(L, 1);
      struct stat buf = {};
      if (stat(path, &buf) == -1) {
        push_error(L);
      } else {
        new_stat(L, buf);
      }
    }

    void impl_fstat(lua_State* L) {
      struct stat buf = {};
      if (fstat(check_fd(L, 1), &buf) == -1) {
        push_error(L);
      } else {
        new_stat(L, buf);
      }
    }

    void impl_umask(lua_State* L) {
      luaX_push(L, umask(luaX_check_integer<mode_t>(L, 1)));
    }
  }

  void initialize_sys_stat(lua_State* L) {
    luaX_set_field(L, -1, "stat", impl_stat);
    luaX_set_field(L, -1, "umask", impl_umask);
  }

  void initialize_fd_stat(lua_State* L) {
    luaX_set_field(L, -1, "fstat", impl_fstat);
  }
}
