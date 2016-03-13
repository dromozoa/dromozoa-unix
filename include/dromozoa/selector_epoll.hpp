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

#ifndef DROMOZOA_SELECTOR_EPOLL_HPP
#define DROMOZOA_SELECTOR_EPOLL_HPP

#include <stddef.h>
#include <time.h>
#include <sys/epoll.h>

#include <vector>

#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/selector.hpp>

namespace dromozoa {
  extern const int SELECTOR_CLOEXEC;

  class selector_epoll : public selector {
  public:
    static int open(size_t size, int flags);
    selector_epoll(int fd, size_t size);
    virtual ~selector_epoll();
    virtual int close();
    virtual bool valid() const;
    virtual int add(int fd, int event);
    virtual int mod(int fd, int event);
    virtual int del(int fd);
    virtual int select(const struct timespec* timeout);
    virtual int event(int i, int& fd, int& event) const;
  private:
    file_descriptor fd_;
    int result_;
    std::vector<struct epoll_event> buffer_;
    selector_epoll(const selector_epoll&);
    selector_epoll& operator=(const selector_epoll&);
  };
}

#endif
