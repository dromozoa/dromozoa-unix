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

#include <fcntl.h>
#include <limits.h>
#include <sys/socket.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/compat_socket.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/ndelay.hpp>
#include <dromozoa/sigmask.hpp>

namespace dromozoa {
  namespace {
#if !defined(SOCK_CLOEXEC) || !defined(SOCK_NONBLOCK)
    struct sock_types {
      static const int value = SOCK_STREAM | SOCK_DGRAM | SOCK_SEQPACKET
#ifdef SOCK_RAW
        | SOCK_RAW
#endif
#ifdef SOCK_RDM
        | SOCK_RDM
#endif
#ifdef SOCK_PACKET
        | SOCK_PACKET
#endif
        ;
    };

    template <int T1, int T2, bool = (T2 <= (INT_MAX >> 2) + 1), bool = (T1 & T2)>
    struct sock_flag_impl {};

    template <int T1, int T2>
    struct sock_flag_impl<T1, T2, true, true>  {
      static const int value = sock_flag_impl<T1, (T2 << 1)>::value;
    };

    template <int T1, int T2, bool T3>
    struct sock_flag_impl<T1, T2, T3, false> {
      static const int value = T2;
    };

    template <int T1, int T2, bool = (T1 & T2)>
    struct sock_flag {};

    template <int T1, int T2>
    struct sock_flag<T1, T2, true> {
      static const int value = sock_flag_impl<T1, (T2 > 0x100 ? T2 : 0x100)>::value;
    };

    template <int T1, int T2>
    struct sock_flag<T1, T2, false> {
      static const int value = T2;
    };
#endif

    struct sock_cloexec {
#ifdef SOCK_CLOEXEC
      static const int value = SOCK_CLOEXEC;
#else
      static const int value = sock_flag<sock_types::value, O_CLOEXEC>::value;
#endif
    };

    struct sock_nonblock {
#ifdef SOCK_NONBLOCK
      static const int value = SOCK_NONBLOCK;
#else
      static const int value = sock_flag<(sock_types::value | sock_cloexec::value), O_NONBLOCK>::value;
#endif
    };
  }

  const int COMPAT_SOCK_CLOEXEC = sock_cloexec::value;
  const int COMPAT_SOCK_NONBLOCK = sock_nonblock::value;

#if defined(SOCK_CLOEXEC) && defined(SOCK_NONBLOCK)
  int compat_socket(int domain, int type, int protocol) {
    return socket(domain, type, protocol);
  }

  int compat_socketpair(int domain, int type, int protocol, int socket_fd[2]) {
    return socketpair(domain, type, protocol, socket_fd);
  }
#else
  int compat_socket(int domain, int type, int protocol) {
    sigset_t mask;
    if (sigmask_block_all_signals(&mask) == -1) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    int flags = type;
    type &= ~(COMPAT_SOCK_CLOEXEC | COMPAT_SOCK_NONBLOCK);

    file_descriptor fd(socket(domain, type, protocol));
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

  int compat_socketpair(int domain, int type, int protocol, int socket_fd[2]) {
    sigset_t mask;
    if (sigmask_block_all_signals(&mask) == -1) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    int flags = type;
    type &= ~(COMPAT_SOCK_CLOEXEC | COMPAT_SOCK_NONBLOCK);

    int fd[2] = { -1, -1 };
    if (socketpair(domain, type, protocol, fd) == -1) {
      return -1;
    }
    file_descriptor fd0(fd[0]);
    file_descriptor fd1(fd[1]);

    if (flags & COMPAT_SOCK_CLOEXEC) {
      if (coe(fd0.get()) == -1) {
        return -1;
      }
      if (coe(fd1.get()) == -1) {
        return -1;
      }
    }
    if (flags & COMPAT_SOCK_NONBLOCK) {
      if (ndelay_on(fd0.get()) == -1) {
        return -1;
      }
      if (ndelay_on(fd1.get()) == -1) {
        return -1;
      }
    }

    socket_fd[0] = fd0.release();
    socket_fd[1] = fd1.release();
    return 0;
  }
#endif
}
