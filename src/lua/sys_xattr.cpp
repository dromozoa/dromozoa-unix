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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "common.hpp"

namespace dromozoa {
  namespace {
#ifdef HAVE_GETXATTR
#define HAVE_COMPAT_GETXATTR 1
#ifdef XATTR_NOFOLLOW
    ssize_t compat_getxattr(const char* path, const char* name, void* value, size_t size) {
      return getxattr(path, name, value, size, 0, 0);
    }
#else
    ssize_t compat_getxattr(const char* path, const char* name, void* value, size_t size) {
      return getxattr(path, name, value, size);
    }
#endif
#endif

#ifdef HAVE_LGETXATTR
#define HAVE_COMPAT_LGETXATTR 1
    ssize_t compat_lgetxattr(const char* path, const char* name, void* value, size_t size) {
      return lgetxattr(path, name, value, size);
    }
#elif defined(HAVE_GETXATTR) && defined(XATTR_NOFOLLOW)
#define HAVE_COMPAT_LGETXATTR 1
    ssize_t compat_lgetxattr(const char* path, const char* name, void* value, size_t size) {
      return getxattr(path, name, value, size, 0, XATTR_NOFOLLOW);
    }
#endif

#ifdef HAVE_SETXATTR
#define HAVE_COMPAT_SETXATTR 1
#ifdef XATTR_NOFOLLOW
    int compat_setxattr(const char* path, const char* name, const void* value, size_t size) {
      return setxattr(path, name, value, size, 0, 0);
    }
#else
    int compat_setxattr(const char* path, const char* name, const void* value, size_t size) {
      return setxattr(path, name, value, size);
    }
#endif
#endif

#ifdef HAVE_LSETXATTR
#define HAVE_COMPAT_LSETXATTR 1
    int compat_lsetxattr(const char* path, const char* name, const void* value, size_t size) {
      return lsetxattr(path, name, value, size);
    }
#elif defined(HAVE_SETXATTR) && defined(XATTR_NOFOLLOW)
#define HAVE_COMPAT_LSETXATTR 1
    int compat_lsetxattr(const char* path, const char* name, const void* value, size_t size) {
      return setxattr(path, name, value, size, 0, XATTR_NOFOLLOW);
    }
#endif

#ifdef HAVE_COMPAT_GETXATTR
    void impl_getxattr(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      ssize_t size = compat_getxattr(path.data(), name.data(), 0, 0);
      if (size == -1) {
        push_error(L);
      } else {
        std::vector<char> buffer(size);
        ssize_t result = compat_getxattr(path.data(), name.data(), buffer.data(), buffer.size());
        if (result == -1) {
          push_error(L);
        } else {
          luaX_push(L, luaX_string_reference(buffer.data(), result));
        }
      }
    }
#endif

#ifdef HAVE_COMPAT_LGETXATTR
    void impl_lgetxattr(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      ssize_t size = compat_lgetxattr(path.data(), name.data(), 0, 0);
      if (size == -1) {
        push_error(L);
      } else {
        std::vector<char> buffer(size);
        ssize_t result = compat_lgetxattr(path.data(), name.data(), buffer.data(), buffer.size());
        if (result == -1) {
          push_error(L);
        } else {
          luaX_push(L, luaX_string_reference(buffer.data(), result));
        }
      }
    }
#endif

#ifdef HAVE_COMPAT_SETXATTR
    void impl_setxattr(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      luaX_string_reference value = luaX_check_string(L, 3);
      if (compat_setxattr(path.data(), name.data(), value.data(), value.size()) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
#endif

#ifdef HAVE_COMPAT_LSETXATTR
    void impl_lsetxattr(lua_State* L) {
      luaX_string_reference path = luaX_check_string(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      luaX_string_reference value = luaX_check_string(L, 3);
      if (compat_lsetxattr(path.data(), name.data(), value.data(), value.size()) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }
#endif
  }

  void initialize_sys_xattr(lua_State* L) {
#ifdef HAVE_COMPAT_GETXATTR
    luaX_set_field(L, -1, "getxattr", impl_getxattr);
#endif
#ifdef HAVE_COMPAT_LGETXATTR
    luaX_set_field(L, -1, "lgetxattr", impl_lgetxattr);
#endif
#ifdef HAVE_COMPAT_SETXATTR
    luaX_set_field(L, -1, "setxattr", impl_setxattr);
#endif
#ifdef HAVE_COMPAT_LSETXATTR
    luaX_set_field(L, -1, "lsetxattr", impl_lsetxattr);
#endif
  }
}
