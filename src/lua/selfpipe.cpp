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

#include <dromozoa/selfpipe.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_open(lua_State* L) {
      if (selfpipe_open() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_close(lua_State* L) {
      if (selfpipe_close() == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_get(lua_State* L) {
      luaX_push(L, selfpipe_get());
    }

    void impl_read(lua_State* L) {
      luaX_push(L, selfpipe_read());
    }
  }

  void initialize_selfpipe(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "open", impl_open);
      luaX_set_field(L, -1, "close", impl_close);
      luaX_set_field(L, -1, "get", impl_get);
      luaX_set_field(L, -1, "read", impl_read);
    }
    luaX_set_field(L, -2, "selfpipe");
  }
}
