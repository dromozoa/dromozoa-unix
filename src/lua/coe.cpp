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

#include <errno.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/compat_strerror.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_coe(luaX_State& LX) {
      if (coe(get_fd(LX.get(), 1)) == -1) {
        // push_failure(LX);
        LX.push(luaX_nil).push(compat_strerror(errno)).push(errno);
      } else {
        // push_success(LX);
        LX.push_value(1);
      }
    }
  }

  void initialize_coe(lua_State* L) {
    luaX_State(L)
      .set_table("coe", impl_coe);
  }
}
