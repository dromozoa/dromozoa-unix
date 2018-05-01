// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_SELFPIPE_HPP
#define DROMOZOA_SELFPIPE_HPP

#include <dromozoa/bind/scoped_ptr.hpp>

namespace dromozoa {
  class selfpipe_impl;

  class selfpipe {
  public:
    static selfpipe_impl* open();
    explicit selfpipe(selfpipe_impl* impl);
    ~selfpipe();
    int close();
    bool valiid() const;
    int get() const;
    int read() const;
  private:
    scoped_ptr<selfpipe_impl> impl_;
    selfpipe(const selfpipe&);
    selfpipe& operator=(const selfpipe&);
  };
}

#endif
