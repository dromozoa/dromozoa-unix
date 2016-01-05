// Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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

extern "C" {
#include <lua.h>
}

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "error.hpp"

namespace dromozoa {
  namespace {
#ifdef HAVE_STRERROR_R
#ifdef STRERROR_R_CHAR_P
    const char* strerror_r(int code, char* buffer, size_t size) {
      return ::strerror_r(code, buffer, size);
    }
#else
    const char* strerror_r(int code, char* buffer, size_t size) {
      int result = ::strerror_r(code, buffer, size);
      if (result == 0) {
        return buffer;
      } else {
        if (result != -1) {
          errno = result;
        }
        return 0;
      }
    }
#endif
#endif
  }

  int push_error(lua_State* L, int code) {
    int save = errno;
    char* buffer = 0;
    size_t size = 32;

    lua_pushnil(L);

    while (true) {
      const char* what = 0;
      errno = 0;
#ifdef HAVE_STRERROR_R
      size = size * 2;
      if (char* b = static_cast<char*>(realloc(buffer, size))) {
        buffer = b;
        what = strerror_r(code, buffer, size);
      } else {
        errno = ENOMEM;
      }
#else
      what = strerror(code);
#endif

      if (what && errno == 0) {
        lua_pushstring(L, what);
        break;
#ifdef HAVE_STRERROR_R
      } else if (errno == ERANGE) {
        continue;
#endif
      } else {
        lua_pushfstring(L, "error number %d", code);
        break;
      }
    }
    free(buffer);

    lua_pushinteger(L, code);
    errno = save;
    return 3;
  }

  void print_error(std::ostream& out, int code) {
    int save = errno;
    char* buffer = 0;
    size_t size = 32;

    while (true) {
      const char* what = 0;
      errno = 0;
#ifdef HAVE_STRERROR_R
      size = size * 2;
      if (char* b = static_cast<char*>(realloc(buffer, size))) {
        buffer = b;
        what = strerror_r(code, buffer, size);
      } else {
        errno = ENOMEM;
      }
#else
      what = strerror(code);
#endif

      if (what && errno == 0) {
        out << what;
        break;
#ifdef HAVE_STRERROR_R
      } else if (errno == ERANGE) {
        continue;
#endif
      } else {
        out << "error number " << code;
        break;
      }
    }
    free(buffer);

    errno = save;
  }
}
