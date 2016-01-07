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

#ifndef DROMOZOA_SELECTOR_HPP
#define DROMOZOA_SELECTOR_HPP

extern "C" {
#include <lua.h>
}

#include <time.h>

namespace dromozoa {
  class selector {
  public:
    virtual ~selector();
    virtual int open(int size, int flags) = 0;
    virtual int close() = 0;
    virtual int get() const = 0;
    virtual int add(int fd, int event) = 0;
    virtual int mod(int fd, int event) = 0;
    virtual int del(int fd) = 0;
    virtual int select(const struct timespec* timeout) = 0;
    virtual int event(int i, int& fd, int& event) const = 0;
  };

  int open_selector(lua_State* L);
}

#endif
