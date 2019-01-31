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

#include <dirent.h>
#include <errno.h>

#include "common.hpp"

namespace dromozoa {
  namespace {
    class dir_handle {
    public:
      explicit dir_handle(DIR* handle) : handle_(handle) {}

      ~dir_handle() {
        if (handle_) {
          errno_saver save_errno;
          if (closedir() == -1) {
            DROMOZOA_UNEXPECTED(compat_strerror(errno));
          }
        }
      }

      int closedir() {
        DIR* handle = handle_;
        handle_ = 0;
        return ::closedir(handle);
      }

      DIR* get() const {
        return handle_;
      }

    private:
      DIR* handle_;
      dir_handle(const dir_handle&);
      dir_handle& operator=(const dir_handle&);
    };

    dir_handle* check_dir_handle(lua_State* L, int arg) {
      return luaX_check_udata<dir_handle>(L, arg, "dromozoa.unix.dir");
    }

    DIR* check_dir(lua_State* L, int arg) {
      return check_dir_handle(L, arg)->get();
    }

    void impl_gc(lua_State* L) {
      check_dir_handle(L, 1)->~dir_handle();
    }

    void impl_closedir(lua_State* L) {
      if (check_dir_handle(L, 1)->closedir() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_readdir(lua_State* L) {
      DIR* dir = check_dir(L, 1);
      errno = 0;
      if (struct dirent* result = readdir(dir)) {
        lua_newtable(L);
        luaX_set_field(L, -1, "d_ino", result->d_ino);
        luaX_set_field(L, -1, "d_name", result->d_name);
      } else {
        if (errno == 0) {
          luaX_push(L, luaX_nil);
        } else {
          push_error(L);
        }
      }
    }

    void impl_rewinddir(lua_State* L) {
      rewinddir(check_dir(L, 1));
      luaX_push_success(L);
    }

    void impl_seekdir(lua_State* L) {
      seekdir(check_dir(L, 1), luaX_check_integer<long>(L, 2));
      luaX_push_success(L);
    }

    void impl_telldir(lua_State* L) {
      luaX_push(L, telldir(check_dir(L, 1)));
    }

    void impl_opendir(lua_State* L) {
      luaX_string_reference dirname = luaX_check_string(L, 1);
      if (DIR* dir = opendir(dirname.data())) {
        luaX_new<dir_handle>(L, dir);
        luaX_set_metatable(L, "dromozoa.unix.dir");
      } else {
        push_error(L);
      }
    }
  }

  void initialize_dirent(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.unix.dir");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "closedir", impl_closedir);
      luaX_set_field(L, -1, "readdir", impl_readdir);
      luaX_set_field(L, -1, "rewinddir", impl_rewinddir);
      luaX_set_field(L, -1, "seekdir", impl_seekdir);
      luaX_set_field(L, -1, "telldir", impl_telldir);
    }
    luaX_set_field(L, -2, "dir");

    luaX_set_field(L, -1, "opendir", impl_opendir);
  }
}
