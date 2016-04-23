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

#include <fcntl.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/compat_accept4.hpp>
#include <dromozoa/compat_socket.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/ndelay.hpp>
#include <dromozoa/sigmask.hpp>

namespace dromozoa {
#ifdef HAVE_ACCEPT4
  int compat_accept4(int socket, struct sockaddr* address, socklen_t* size_ptr, int flags) {
    return accept4(socket, address, size_ptr, flags);
  }
#else
  int compat_accept4(int socket, struct sockaddr* address, socklen_t* size_ptr, int flags) {
    sigset_t mask;
    if (sigmask_block_all_signals(&mask) == -1) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    file_descriptor fd(accept(socket, address, size_ptr));
    if (!fd.valid()) {
      return -1;
    }

    if (flags & COMPAT_SOCK_CLOEXEC) {
      if (coe(fd.get()) == -1) {
        return -1;
      }
    }
    if (flags & COMPAT_SOCK_NONBLOCK) {
      if (ndelay_on(fd.get()) == -1) {
        return -1;
      }
    }

    return fd.release();
  }
#endif
}
