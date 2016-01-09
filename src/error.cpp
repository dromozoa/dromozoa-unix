// Copyright (C) 2015,2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "error.hpp"
#include "set_field.hpp"

namespace dromozoa {
#ifdef HAVE_STRERROR_R
  namespace {
    const char* wrap_strerror_r_result(const char* result, char*) {
      return result;
    }

    const char* wrap_strerror_r_result(int result, char* buffer) {
      if (result == 0) {
        return buffer;
      } else {
        if (result != -1) {
          errno = result;
        }
        return 0;
      }
    }

    const char* wrap_strerror_r(int code, char* buffer, size_t size) {
      return wrap_strerror_r_result(strerror_r(code, buffer, size), buffer);
    }
  }
#endif

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
        what = wrap_strerror_r(code, buffer, size);
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
        what = wrap_strerror_r(code, buffer, size);
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

  void initialize_error(lua_State* L) {
    DROMOZOA_SET_FIELD(L, EAGAIN);
    DROMOZOA_SET_FIELD(L, EINPROGRESS);
    DROMOZOA_SET_FIELD(L, EINTR);
    DROMOZOA_SET_FIELD(L, ENOENT);
    DROMOZOA_SET_FIELD(L, EPIPE);
    DROMOZOA_SET_FIELD(L, EWOULDBLOCK);
  }
}
