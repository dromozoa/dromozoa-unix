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

#include <signal.h>

#include <iostream>

#include <dromozoa/sigmask.hpp>
#include <dromozoa/signal_mask.hpp>

namespace dromozoa {
  scoped_signal_mask::scoped_signal_mask() : masked_() {}

  scoped_signal_mask::~scoped_signal_mask() {
    if (masked_) {
      if (compat_sigmask(SIG_SETMASK, &mask_, 0) == -1) {
        // [TODO]
        // int code = errno;
        // std::cerr << "[dromozoa-unix] cannot reset signal mask: ";
        // print_error(std::cerr, code);
        // std::cerr << std::endl;
      }
    }
  }

  int scoped_signal_mask::block_all_signals() {
    sigset_t mask;
    if (sigfillset(&mask) == -1) {
      return -1;
    }
    if (compat_sigmask(SIG_BLOCK, &mask, &mask_) == -1) {
      return -1;
    }
    masked_ = true;
    return 0;
  }
}
