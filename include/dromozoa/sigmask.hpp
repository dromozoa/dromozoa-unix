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

#ifndef DROMOZOA_SIGMASK_HPP
#define DROMOZOA_SIGMASK_HPP

#include <signal.h>

namespace dromozoa {
  int sigmask_block_all_signals(sigset_t* old_mask);
  int sigmask_unblock_all_signals(sigset_t* old_mask);

  class sigmask_saver {
  public:
    explicit sigmask_saver(const sigset_t& mask);
    ~sigmask_saver();
  private:
    sigset_t mask_;
    sigmask_saver(const sigmask_saver&);
    sigmask_saver& operator=(const sigmask_saver&);
  };
}

#endif
