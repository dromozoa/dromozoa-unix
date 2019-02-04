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

#include <errno.h>
#include <string.h>

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
    int compat_setxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
      return setxattr(path, name, value, size, 0, flags);
    }
#else
    int compat_setxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
      return setxattr(path, name, value, size, flags);
    }
#endif
#endif

#ifdef HAVE_LSETXATTR
#define HAVE_COMPAT_LSETXATTR 1
    int compat_lsetxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
      return lsetxattr(path, name, value, size, flags);
    }
#elif defined(HAVE_SETXATTR) && defined(XATTR_NOFOLLOW)
#define HAVE_COMPAT_LSETXATTR 1
    int compat_lsetxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
      return setxattr(path, name, value, size, 0, flags | XATTR_NOFOLLOW);
    }
#endif

#ifdef HAVE_REMOVEXATTR
#define HAVE_COMPAT_REMOVEXATTR 1
#ifdef XATTR_NOFOLLOW
    int compat_removexattr(const char* path, const char* name) {
      return removexattr(path, name, 0);
    }
#else
    int compat_removexattr(const char* path, const char* name) {
      return removexattr(path, name);
    }
#endif
#endif

#ifdef HAVE_LREMOVEXATTR
#define HAVE_COMPAT_LREMOVEXATTR 1
    int compat_lremovexattr(const char* path, const char* name) {
      return lremovexattr(path, name);
    }
#elif defined(HAVE_REMOVEXATTR) && defined(XATTR_NOFOLLOW)
#define HAVE_COMPAT_LREMOVEXATTR 1
    int compat_lremovexattr(const char* path, const char* name) {
      return removexattr(path, name, XATTR_NOFOLLOW);
    }
#endif

#ifdef HAVE_LISTXATTR
#define HAVE_COMPAT_LISTXATTR 1
#ifdef XATTR_NOFOLLOW
    ssize_t compat_listxattr(const char* path, char* buffer, size_t size) {
      return listxattr(path, buffer, size, 0);
    }
#else
    ssize_t compat_listxattr(const char* path, char* buffer, size_t size) {
      return listxattr(path, buffer, size);
    }
#endif
#endif

#ifdef HAVE_LLISTXATTR
#define HAVE_COMPAT_LLISTXATTR 1
    ssize_t compat_llistxattr(const char* path, char* buffer, size_t size) {
      return llistxattr(path, buffer, size);
    }
#elif defined(HAVE_LISTXATTR) && defined(XATTR_NOFOLLOW)
#define HAVE_COMPAT_LLISTXATTR 1
    ssize_t compat_llistxattr(const char* path, char* buffer, size_t size) {
      return listxattr(path, buffer, size, XATTR_NOFOLLOW);
    }
#endif

    template <class T>
    void impl_getxattr_(lua_State* L, T f) {
      luaX_string_reference path = luaX_check_string(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      ssize_t size = f(path.data(), name.data(), 0, 0);
      if (size == -1) {
        push_error(L);
      } else {
        std::vector<char> buffer(size);
        ssize_t result = f(path.data(), name.data(), buffer.data(), buffer.size());
        if (result == -1) {
          push_error(L);
        } else {
          luaX_push(L, luaX_string_reference(buffer.data(), result));
        }
      }
    }

    template <class T>
    void impl_setxattr_(lua_State* L, T f) {
      luaX_string_reference path = luaX_check_string(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      luaX_string_reference value = luaX_check_string(L, 3);
      int flags = luaX_opt_integer<int>(L, 4, 0);
      if (f(path.data(), name.data(), value.data(), value.size(), flags) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    template <class T>
    void impl_removexattr_(lua_State* L, T f) {
      luaX_string_reference path = luaX_check_string(L, 1);
      luaX_string_reference name = luaX_check_string(L, 2);
      if (f(path.data(), name.data()) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    template <class T>
    void impl_listxattr_(lua_State* L, T f) {
      luaX_string_reference path = luaX_check_string(L, 1);
      ssize_t size = f(path.data(), 0, 0);
      if (size == -1) {
        push_error(L);
      } else if (size == 0) {
        lua_newtable(L);
      } else {
        std::vector<char> buffer(size);
        ssize_t result = f(path.data(), &buffer[0], buffer.size());
        if (result == -1) {
          push_error(L);
        } else if (result == 0) {
          lua_newtable(L);
        } else {
          lua_newtable(L);
          int i = 1;
          ssize_t j = 0;
          while (j < result) {
            const char* data = &buffer[j];
            luaX_string_reference name(data, strlen(data));
            luaX_set_field(L, -1, i, name);
            ++i;
            j += name.size() + 1;
          }
        }
      }
    }

#ifdef HAVE_COMPAT_GETXATTR
    void impl_getxattr(lua_State* L) {
      impl_getxattr_(L, compat_getxattr);
    }
#endif

#ifdef HAVE_COMPAT_LGETXATTR
    void impl_lgetxattr(lua_State* L) {
      impl_getxattr_(L, compat_lgetxattr);
    }
#endif

#ifdef HAVE_COMPAT_SETXATTR
    void impl_setxattr(lua_State* L) {
      impl_setxattr_(L, compat_setxattr);
    }
#endif

#ifdef HAVE_COMPAT_LSETXATTR
    void impl_lsetxattr(lua_State* L) {
      impl_setxattr_(L, compat_lsetxattr);
    }
#endif

#ifdef HAVE_COMPAT_REMOVEXATTR
    void impl_removexattr(lua_State* L) {
      impl_removexattr_(L, compat_removexattr);
    }
#endif

#ifdef HAVE_COMPAT_LREMOVEXATTR
    void impl_lremovexattr(lua_State* L) {
      impl_removexattr_(L, compat_lremovexattr);
    }
#endif

#ifdef HAVE_COMPAT_LISTXATTR
    void impl_listxattr(lua_State* L) {
      impl_listxattr_(L, compat_listxattr);
    }
#endif

#ifdef HAVE_COMPAT_LLISTXATTR
    void impl_llistxattr(lua_State* L) {
      impl_listxattr_(L, compat_llistxattr);
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
#ifdef HAVE_COMPAT_REMOVEXATTR
    luaX_set_field(L, -1, "removexattr", impl_removexattr);
#endif
#ifdef HAVE_COMPAT_LREMOVEXATTR
    luaX_set_field(L, -1, "lremovexattr", impl_lremovexattr);
#endif
#ifdef HAVE_COMPAT_LISTXATTR
    luaX_set_field(L, -1, "listxattr", impl_listxattr);
#endif
#ifdef HAVE_COMPAT_LLISTXATTR
    luaX_set_field(L, -1, "llistxattr", impl_llistxattr);
#endif

#ifdef XATTR_CREATE
    luaX_set_field<int>(L, -1, "XATTR_CREATE", XATTR_CREATE);
#endif
#ifdef XATTR_REPLACE
    luaX_set_field<int>(L, -1, "XATTR_REPLACE", XATTR_REPLACE);
#endif

#ifdef ENOATTR
    luaX_set_field(L, -1, "ENOATTR", ENOATTR);
#endif
  }
}
