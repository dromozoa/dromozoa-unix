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

#ifndef DROMOZOA_ARGUMENT_VECTOR_HPP
#define DROMOZOA_ARGUMENT_VECTOR_HPP

extern "C" {
#include <lua.h>
}

#include <string>
#include <vector>

namespace dromozoa {
  class argument_vector {
  public:
    argument_vector(lua_State* L, int n);
    const char* const* get() const;
  private:
    std::vector<std::string> str_;
    std::vector<const char*> ptr_;
    argument_vector(const argument_vector&);
    argument_vector& operator=(const argument_vector&);
  };
}

#endif
