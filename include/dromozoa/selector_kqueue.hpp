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
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <vector>

#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/selector.hpp>

namespace dromozoa {
  extern const int SELECTOR_CLOEXEC;

  class selector_kqueue : public selector {
  public:
    static int open(int flags);
    selector_kqueue(int fd);
    virtual ~selector_kqueue();
    virtual int close();
    virtual bool valid() const;
    virtual int get() const;
    virtual int add(int fd, int event);
    virtual int mod(int fd, int event);
    virtual int del(int fd);
    virtual int select(const struct timespec* timeout);
    virtual int event(int i, int& fd, int& event) const;
  private:
    file_descriptor fd_;
    int result_;
    std::vector<struct kevent> buffer_;
    selector_kqueue(const selector_kqueue&);
    selector_kqueue& operator=(const selector_kqueue&);
  };
}

#endif
