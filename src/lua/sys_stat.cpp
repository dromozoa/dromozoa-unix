// Copyright (C) 2016,2017,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>

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

#if defined(HAVE_STRUCT_STAT_ST_ATIM)
      new_timespec(L, buf.st_atim, TIMESPEC_TYPE_REALTIME);
#elif defined(HAVE_STRUCT_STAT_ST_ATIMESPEC)
      new_timespec(L, buf.st_atimespec, TIMESPEC_TYPE_REALTIME);
#else
      struct timespec atim = {};
      atim.tv_sec = buf.st_atime;
      new_timespec(L, atim, TIMESPEC_TYPE_REALTIME);
#endif
      luaX_set_field(L, -2, "st_atim");

#if defined(HAVE_STRUCT_STAT_ST_MTIM)
      new_timespec(L, buf.st_mtim, TIMESPEC_TYPE_REALTIME);
#elif defined(HAVE_STRUCT_STAT_ST_MTIMESPEC)
      new_timespec(L, buf.st_mtimespec, TIMESPEC_TYPE_REALTIME);
#else
      struct timespec mtim = {};
      mtim.tv_sec = buf.st_mtime;
      new_timespec(L, mtim, TIMESPEC_TYPE_REALTIME);
#endif
      luaX_set_field(L, -2, "st_mtim");

#if defined(HAVE_STRUCT_STAT_ST_CTIM)
      new_timespec(L, buf.st_ctim, TIMESPEC_TYPE_REALTIME);
#elif defined(HAVE_STRUCT_STAT_ST_CTIMESPEC)
      new_timespec(L, buf.st_ctimespec, TIMESPEC_TYPE_REALTIME);
#else
      struct timespec ctim = {};
      ctim.tv_sec = buf.st_ctime;
      new_timespec(L, ctim, TIMESPEC_TYPE_REALTIME);
#endif
      luaX_set_field(L, -2, "st_ctim");

      luaX_set_field(L, -1, "st_blksize", buf.st_blksize);
      luaX_set_field(L, -1, "st_blocks", buf.st_blocks);
    }

    void impl_stat(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      struct stat buf = {};
      if (stat(path.data(), &buf) == -1) {
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

    void impl_mkdir(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      mode_t mode = luaX_opt_integer<mode_t>(L, 2, 0777);
      if (mkdir(path.data(), mode) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_mkfifo(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      mode_t mode = luaX_opt_integer<mode_t>(L, 2, 0666);
      if (mkfifo(path.data(), mode) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_chmod(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      mode_t mode = luaX_check_integer<mode_t>(L, 2);
      if (chmod(path.data(), mode) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_utimensat(lua_State* L) {
      int fd = check_fd(L, 1);
      luaX_string_reference path = luaX_check_string(L, 2);
      bool atime_isnoneornil = lua_isnoneornil(L, 3);
      bool mtime_isnoneornil = lua_isnoneornil(L, 4);
      int flags = luaX_opt_integer<int>(L, 5, 0);

      int result = -1;
      if (atime_isnoneornil && mtime_isnoneornil) {
        result = utimensat(fd, path.data(), 0, flags);
      } else {
        struct timespec times[2] = {};
        if (atime_isnoneornil) {
          times[0].tv_nsec = UTIME_NOW;
        } else {
          check_timespec(L, 3, times[0], false);
        }
        if (mtime_isnoneornil) {
          times[1].tv_nsec = UTIME_NOW;
        } else {
          check_timespec(L, 4, times[1], false);
        }
        result = utimensat(fd, path.data(), times, flags);
      }

      if (result == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_mknod(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      mode_t mode = luaX_check_integer<mode_t>(L, 2);
      dev_t dev = luaX_opt_integer<dev_t>(L, 3, 0);
      if (mknod(path.data(), mode, dev) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
  }

  void initialize_sys_stat(lua_State* L) {
    luaX_set_field(L, -1, "stat", impl_stat);
    luaX_set_field(L, -1, "umask", impl_umask);
    luaX_set_field(L, -1, "mkdir", impl_mkdir);
    luaX_set_field(L, -1, "mkfifo", impl_mkfifo);
    luaX_set_field(L, -1, "chmod", impl_chmod);
    luaX_set_field(L, -1, "utimensat", impl_utimensat);
    luaX_set_field(L, -1, "mknod", impl_mknod);

    luaX_set_field<mode_t>(L, -1, "S_IFMT", S_IFMT);
    luaX_set_field<mode_t>(L, -1, "S_IFBLK", S_IFBLK);
    luaX_set_field<mode_t>(L, -1, "S_IFCHR", S_IFCHR);
    luaX_set_field<mode_t>(L, -1, "S_IFIFO", S_IFIFO);
    luaX_set_field<mode_t>(L, -1, "S_IFREG", S_IFREG);
    luaX_set_field<mode_t>(L, -1, "S_IFDIR", S_IFDIR);
    luaX_set_field<mode_t>(L, -1, "S_IFLNK", S_IFLNK);
    luaX_set_field<mode_t>(L, -1, "S_IFSOCK", S_IFSOCK);

    luaX_set_field(L, -1, "UTIME_NOW", UTIME_NOW);
    luaX_set_field(L, -1, "UTIME_OMIT", UTIME_OMIT);
  }

  void initialize_fd_stat(lua_State* L) {
    luaX_set_field(L, -1, "fstat", impl_fstat);
  }
}
