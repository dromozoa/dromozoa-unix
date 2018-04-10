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

#include <dromozoa/file_descriptor.hpp>

namespace dromozoa {
  class selfpipe {
  public:
    selfpipe();
    ~selfpipe();
    int close();
    bool valiid();
    int get() const;
    int read() const;
  private:
    file_descriptor fd_;
    selfpipe(const selfpipe&);
    selfpipe& operator=(const selfpipe&);
  };

  int selfpipe_open();
  int selfpipe_close();
  bool selfpipe_valid();
  int selfpipe_get();
  int selfpipe_read();
}

#endif
