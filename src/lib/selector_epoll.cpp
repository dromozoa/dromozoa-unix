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
#include <sys/epoll.h>

#include <dromozoa/coe.hpp>
#include <dromozoa/selector_epoll.hpp>
#include <dromozoa/sigmask.hpp>

namespace dromozoa {
#ifdef EPOLL_CLOEXEC
  const int SELECTOR_CLOEXEC = EPOLL_CLOEXEC;
#else
  const int SELECTOR_CLOEXEC = O_CLOEXEC;
#endif

#ifdef HAVE_EPOLL_CREATE1
  int selector_epoll::open(size_t, int flags) {
    return epoll_create1(flags);
  }
#else
  int selector_epoll::open(size_t size, int flags) {
    sigset_t mask;
    if (sigmask_block_all_signals(&mask) == -1) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    file_descriptor fd(epoll_create(size));
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
#endif

  selector_epoll::selector_epoll(int fd, size_t size) : fd_(fd), result_(-1) {
    buffer_.resize(size);
  }

  selector_epoll::~selector_epoll() {}

  int selector_epoll::close() {
    return fd_.close();
  }

  bool selector_epoll::valid() const {
    return fd_.valid();
  }

  int selector_epoll::add(int fd, int event) {
    struct epoll_event ev = {};
    ev.data.fd = fd;
    if (event & SELECTOR_READ) {
      ev.events |= EPOLLIN;
    }
    if (event & SELECTOR_WRITE) {
      ev.events |= EPOLLOUT;
    }
    return epoll_ctl(fd_.get(), EPOLL_CTL_ADD, fd, &ev);
  }

  int selector_epoll::mod(int fd, int event) {
    struct epoll_event ev = {};
    ev.data.fd = fd;
    if (event & SELECTOR_READ) {
      ev.events |= EPOLLIN;
    }
    if (event & SELECTOR_WRITE) {
      ev.events |= EPOLLOUT;
    }
    return epoll_ctl(fd_.get(), EPOLL_CTL_MOD, fd, &ev);
  }

  int selector_epoll::del(int fd) {
    struct epoll_event ev = {};
    return epoll_ctl(fd_.get(), EPOLL_CTL_DEL, fd, &ev);
  }

  int selector_epoll::select(const struct timespec* timeout) {
    int t = -1;
    if (timeout) {
      t = timeout->tv_sec * 1000 + timeout->tv_nsec / 1000000;
    }
    if (result_ == static_cast<int>(buffer_.size())) {
      buffer_.resize(result_ * 2);
    }
    result_ = epoll_wait(fd_.get(), &buffer_[0], buffer_.size(), t);
    return result_;
  }

  int selector_epoll::event(int i, int& fd, int& event) const {
    if (0 <= i && i < result_) {
      const struct epoll_event& ev = buffer_[i];
      fd = ev.data.fd;
      event = 0;
      if (ev.events & EPOLLIN) {
        event |= SELECTOR_READ;
      }
      if (ev.events & EPOLLOUT) {
        event |= SELECTOR_WRITE;
      }
      return 0;
    } else {
      errno = EINVAL;
      return -1;
    }
  }
}
