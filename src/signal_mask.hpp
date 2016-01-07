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

#ifndef DROMOZOA_SIGNAL_MASK_HPP
#define DROMOZOA_SIGNAL_MASK_HPP

#include <signal.h>

namespace dromozoa {
  int signal_mask(int how, const sigset_t* new_mask, sigset_t* old_mask);

  class scoped_signal_mask {
  public:
    scoped_signal_mask();
    ~scoped_signal_mask();
    int block_all_signals();
  private:
    bool masked_;
    sigset_t mask_;
    scoped_signal_mask(const scoped_signal_mask&);
    scoped_signal_mask& operator=(const scoped_signal_mask&);
  };
}

#endif
