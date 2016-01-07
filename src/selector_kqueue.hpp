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

#ifndef DROMOZOA_SELECTOR_KQUEUE_HPP
#define DROMOZOA_SELECTOR_KQUEUE_HPP

#include <time.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <vector>

#include "selector.hpp"

namespace dromozoa {
  class selector_kqueue : public selector {
  public:
    selector_kqueue();
    virtual ~selector_kqueue();
    virtual int open(int size, int flags);
    virtual int close();
    virtual int get() const;
    virtual int add(int fd, int event);
    virtual int mod(int fd, int event);
    virtual int del(int fd);
    virtual int select(const struct timespec* timeout);
    virtual int event(int i, int& fd, int& event) const;
  private:
    int fd_;
    int result_;
    std::vector<struct kevent> buffer_;
    selector_kqueue(const selector_kqueue&);
    selector_kqueue& operator=(const selector_kqueue&);
  };
}

#endif
