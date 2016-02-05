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
#include <lauxlib.h>
}

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "dromozoa/bind.hpp"

#include "error.hpp"

namespace dromozoa {
  using bind::function;
  using bind::push_success;

  int push_resource_unavailable_try_again(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EAGAIN));
    return 1;
  }

  int push_operation_in_progress(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EINPROGRESS));
    return 1;
  }

  int push_interrupted(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EINTR));
    return 1;
  }

  int push_broken_pipe(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(EPIPE));
    return 1;
  }

  int push_timed_out(lua_State* L) {
    lua_pushlightuserdata(L, reinterpret_cast<void*>(ETIMEDOUT));
    return 1;
  }

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

  namespace {
    int push_error_impl(lua_State* L, int code) {
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

      return 1;
    }
  }

  int push_error(lua_State* L, int code) {
    int save = errno;
    lua_pushnil(L);
    push_error_impl(L, code);
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

  namespace {
    int impl_strerror(lua_State* L) {
      int save = errno;
      int code = luaL_optinteger(L, 1, save);
      push_error_impl(L, code);
      errno = save;
      return 1;
    }

    int impl_set_errno(lua_State* L) {
      int code = luaL_checkinteger(L, 1);
      errno = code;
      return push_success(L);
    }

    int impl_get_errno(lua_State* L) {
      int code = errno;
      lua_pushinteger(L, code);
      return 1;
    }
  }

  void initialize_error(lua_State* L) {
    function<impl_strerror>::set_field(L, "strerror");
    function<impl_set_errno>::set_field(L, "set_errno");
    function<impl_get_errno>::set_field(L, "get_errno");

    DROMOZOA_BIND_SET_FIELD(L, EAGAIN);
    DROMOZOA_BIND_SET_FIELD(L, EINPROGRESS);
    DROMOZOA_BIND_SET_FIELD(L, EINTR);
    DROMOZOA_BIND_SET_FIELD(L, ENOENT);
    DROMOZOA_BIND_SET_FIELD(L, EPIPE);
    DROMOZOA_BIND_SET_FIELD(L, ETIMEDOUT);
    DROMOZOA_BIND_SET_FIELD(L, EWOULDBLOCK);

    push_resource_unavailable_try_again(L);
    lua_setfield(L, -2, "resource_unavailable_try_again");

    push_operation_in_progress(L);
    lua_setfield(L, -2, "operation_in_progress");

    push_interrupted(L);
    lua_setfield(L, -2, "interrupted");

    push_broken_pipe(L);
    lua_setfield(L, -2, "broken_pipe");

    push_timed_out(L);
    lua_setfield(L, -2, "timed_out");
  }
}
