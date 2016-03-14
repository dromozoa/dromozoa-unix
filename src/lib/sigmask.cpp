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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <signal.h>

#include <iostream>

#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/sigmask.hpp>
#include <dromozoa/unexpected.hpp>

namespace dromozoa {
#ifdef HAVE_PTHREAD
  int compat_sigmask(int how, const sigset_t* new_mask, sigset_t* old_mask) {
    return pthread_sigmask(how, new_mask, old_mask);
  }
#else
  int compat_sigmask(int how, const sigset_t* new_mask, sigset_t* old_mask) {
    return sigprocmask(how, new_mask, old_mask);
  }
#endif

  int sigmask_block_all_signals(sigset_t* old_mask) {
    sigset_t new_mask;
    if (sigfillset(&new_mask) == -1) {
      return -1;
    }
    if (compat_sigmask(SIG_SETMASK, &new_mask, old_mask) == -1) {
      return -1;
    }
    return 0;
  }

  int sigmask_unblock_all_signals(sigset_t* old_mask) {
    sigset_t new_mask;
    if (sigemptyset(&new_mask) == -1) {
      return -1;
    }
    if (compat_sigmask(SIG_SETMASK, &new_mask, old_mask) == -1) {
      return -1;
    }
    return 0;
  }

  sigmask_saver::sigmask_saver(const sigset_t& mask) : mask_(mask) {}

  sigmask_saver::~sigmask_saver() {
    if (compat_sigmask(SIG_SETMASK, &mask_, 0) == -1) {
      DROMOZOA_UNEXPECTED(compat_strerror(errno));
    }
  }
}
