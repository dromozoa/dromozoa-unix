// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <time.h>

#include <dromozoa/scoped_ptr.hpp>

namespace dromozoa {
  extern const int SELECTOR_READ;
  extern const int SELECTOR_WRITE;
  extern const int SELECTOR_CLOEXEC;

  class selector_impl {
  public:
    virtual ~selector_impl() = 0;
    virtual int open(int flags) = 0;
    virtual int close() = 0;
    virtual bool valid() const = 0;
    virtual int get() const = 0;
    virtual int add(int fd, int event) = 0;
    virtual int mod(int fd, int event) = 0;
    virtual int del(int fd) = 0;
    virtual int select(const struct timespec* timeout) = 0;
    virtual int event(int i, int& fd, int& event) const = 0;
  };

  class selector {
  public:
    static selector_impl* open(int flags);
    explicit selector(selector_impl* impl);
    ~selector();
    int close();
    bool valid() const;
    int get() const;
    int add(int fd, int event);
    int mod(int fd, int event);
    int del(int fd);
    int select(const struct timespec* timeout);
    int event(int i, int& fd, int& event) const;
  private:
    scoped_ptr<selector_impl> impl_;
    selector(const selector&);
    selector& operator=(const selector&);
  };
}

#endif
