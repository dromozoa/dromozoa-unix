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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include <lua.h>
}

#ifdef HAVE_FLOCK
#include <sys/file.h>
#endif

#include <errno.h>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "fd.hpp"

namespace dromozoa {
  using bind::function;
  using bind::push_success;

  namespace {
#ifdef HAVE_FLOCK
    int impl_lock_ex(lua_State* L) {
      if (flock(get_fd(L, 1), LOCK_EX) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_lock_exnb(lua_State* L) {
      if (flock(get_fd(L, 1), LOCK_EX | LOCK_NB) == -1) {
        int code = errno;
        if (code == EWOULDBLOCK) {
          return push_resource_unavailable_try_again(L);
        } else {
          return push_error(L);
        }
      } else {
        return push_success(L);
      }
    }

    int impl_lock_un(lua_State* L) {
      if (flock(get_fd(L, 1), LOCK_UN) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }
#endif
  }

  void initialize_lock(lua_State* L) {
    function<impl_lock_ex>::set_field(L, "lock_ex");
    function<impl_lock_exnb>::set_field(L, "lock_exnb");
    function<impl_lock_un>::set_field(L, "lock_un");
  }
}
