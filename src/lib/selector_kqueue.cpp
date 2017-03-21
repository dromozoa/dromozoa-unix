// Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include <stddef.h>
#include <fcntl.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/selector_kqueue.hpp>
#include <dromozoa/sigmask.hpp>

namespace dromozoa {
  namespace {
    static const size_t INITIAL_BUFFER_SIZE = 64;
    static const int MAX_BUFFER_SIZE = 8192;
  }

  const int SELECTOR_CLOEXEC = O_CLOEXEC;

  int selector_kqueue::open(int flags) {
    sigset_t mask;
    if (sigmask_block_all_signals(&mask) == -1) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    file_descriptor fd(kqueue());
    if (!fd.valid()) {
      return -1;
    }

    if (flags & SELECTOR_CLOEXEC) {
      if (coe(fd.get()) == -1) {
        return -1;
      }
    }

    return fd.release();
  }

  selector_kqueue::selector_kqueue(int fd) : fd_(fd), result_(-1), buffer_(INITIAL_BUFFER_SIZE) {}

  selector_kqueue::~selector_kqueue() {}

  int selector_kqueue::close() {
    return fd_.close();
  }

  bool selector_kqueue::valid() const {
    return fd_.valid();
  }

  int selector_kqueue::get() const {
    return fd_.get();
  }

  int selector_kqueue::add(int fd, int event) {
    struct kevent kev[2];
    if (event & SELECTOR_READ) {
      EV_SET(kev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev, fd, EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, 0);
    }
    if (event & SELECTOR_WRITE) {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, 0);
    }
    int result = kevent(fd_.get(), kev, 2, 0, 0, 0);
    if (result == -1 && errno == EPIPE) {
      result = 0;
    }
    return result;
  }

  int selector_kqueue::mod(int fd, int event) {
    struct kevent kev[2];
    if (event & SELECTOR_READ) {
      EV_SET(kev, fd, EVFILT_READ, EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev, fd, EVFILT_READ, EV_DISABLE, 0, 0, 0);
    }
    if (event & SELECTOR_WRITE) {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_ENABLE, 0, 0, 0);
    } else {
      EV_SET(kev + 1, fd, EVFILT_WRITE, EV_DISABLE, 0, 0, 0);
    }
    int result = kevent(fd_.get(), kev, 2, 0, 0, 0);
    if (result == -1 && errno == EPIPE) {
      result = 0;
    }
    return result;
  }

  int selector_kqueue::del(int fd) {
    struct kevent kev[2];
    EV_SET(kev, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
    EV_SET(kev + 1, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
    int result = kevent(fd_.get(), kev, 2, 0, 0, 0);
    if (result == -1 && errno == ENOENT) {
      result = 0;
    }
    return result;
  }

  int selector_kqueue::select(const struct timespec* timeout) {
    int size = buffer_.size();
    if (size == result_ && size < MAX_BUFFER_SIZE) {
      size *= 2;
      buffer_.resize(size);
    }
    result_ = kevent(fd_.get(), 0, 0, &buffer_[0], size, timeout);
    return result_;
  }

  int selector_kqueue::event(int i, int& fd, int& event) const {
    if (0 <= i && i < result_) {
      const struct kevent& kev = buffer_[i];
      fd = kev.ident;
      switch (kev.filter) {
        case EVFILT_READ:
          event = SELECTOR_READ;
          break;
        case EVFILT_WRITE:
          event = SELECTOR_WRITE;
          break;
        default:
          event = 0;
      }
      return 0;
    } else {
      errno = EINVAL;
      return -1;
    }
  }
}
