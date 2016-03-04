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

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <dromozoa/coe.hpp>

#include "coe.hpp"
#include "selector.hpp"
#include "selector_kqueue.hpp"

namespace dromozoa {
  int open_selector(int, int flags) {
    int fd = kqueue();
    if (fd == -1) {
      return -1;
    }
    if (flags & O_CLOEXEC) {
      if (coe(fd) == -1) {
        close(fd);
        return -1;
      }
    }
    return fd;
  }

  selector::selector(int fd, int size) : fd_(fd), result_(-1) {
    try {
      buffer_.resize(size);
    } catch (...) {
      close();
      throw;
    }
  }

  selector::~selector() {
    if (fd_ != -1) {
      close();
    }
  }

  int selector::close() {
    buffer_.clear();
    result_ = -1;
    int fd = fd_;
    fd_ = -1;
    return ::close(fd);
  }

  int selector::get() const {
    return fd_;
  }

  int selector::add(int fd, int event) {
    struct kevent kev[2];
    if (event & 1) {
      EV_SET(kev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev, fd, EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, 0);
    }
    if (event & 2) {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, 0);
    }
    return kevent(fd_, kev, 2, 0, 0, 0);
  }

  int selector::mod(int fd, int event) {
    struct kevent kev[2];
    if (event & 1) {
      EV_SET(kev, fd, EVFILT_READ, EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev, fd, EVFILT_READ, EV_DISABLE, 0, 0, 0);
    }
    if (event & 2) {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_DISABLE, 0, 0, 0);
    }
    return kevent(fd_, kev, 2, 0, 0, 0);
  }

  int selector::del(int fd) {
    struct kevent kev[2];
    EV_SET(kev, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
    EV_SET(kev + 1, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
    return kevent(fd_, kev, 2, 0, 0, 0);
  }

  int selector::select(const struct timespec* timeout) {
    result_ = kevent(fd_, 0, 0, &buffer_[0], buffer_.size(), timeout);
    return result_;
  }

  int selector::event(int i, int& fd, int& event) const {
    if (0 <= i && i < result_) {
      const struct kevent& kev = buffer_[i];
      fd = kev.ident;
      switch (kev.filter) {
        case EVFILT_READ:
          event = 1;
          break;
        case EVFILT_WRITE:
          event = 2;
          break;
        default:
          event = 0;
      }
      return 0;
    } else {
      errno = ERANGE;
      return -1;
    }
  }
}
