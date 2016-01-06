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

extern "C" {
#include <lua.h>
}

#include <string>
#include <vector>

#include "argument_vector.hpp"

namespace dromozoa {
  argument_vector::argument_vector(lua_State* L, int n) {
    for (int i = 1; ; ++i) {
      lua_pushinteger(L, i);
      lua_gettable(L, n);
      if (const char* p = lua_tostring(L, -1)) {
        str_.push_back(p);
        lua_pop(L, 1);
      } else {
        lua_pop(L, 1);
        break;
      }
    }
    ptr_.reserve(str_.size() + 1);
    std::vector<std::string>::const_iterator i = str_.begin();
    std::vector<std::string>::const_iterator end = str_.end();
    for (; i != end; ++i) {
      ptr_.push_back((*i).c_str());
    }
    ptr_.push_back(0);
  }

  const char* const* argument_vector::get() const {
    return &ptr_[0];
  }
}
