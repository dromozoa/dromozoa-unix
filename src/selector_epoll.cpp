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
#include <sys/epoll.h>

#include "coe.hpp"
#include "selector_epoll.hpp"

namespace dromozoa {
  selector_epoll::selector_epoll() : fd_(-1), result_(-1) {}

  selector_epoll::~selector_epoll() {
    if (fd_ != -1) {
      close();
    }
  }

  int selector_epoll::open(int size, int flags) {
    buffer_.resize(size);

    int fd = -1;
#ifdef HAVE_EPOLL_CREATE1
    if (flags & O_CLOEXEC) {
      fd = epoll_create1(EPOLL_CLOEXEC);
    } else {
      fd = epoll_create1(0);
    }
    if (fd == -1) {
      return -1;
    }
#else
    fd = epoll_create(size);
    if (fd == -1) {
      return -1;
    }
    if (flags & O_CLOEXEC) {
      if (coe(fd) == -1) {
        ::close(fd);
        return -1;
      }
    }
#endif

    fd_ = fd;
    return 0;
  };

  int selector_epoll::close() {
    int fd = fd_;
    fd_ = -1;
    return ::close(fd);
  }

  int selector_epoll::get() const {
    return fd_;
  }

  int selector_epoll::add(int fd, int event) {
    struct epoll_event ev = {};
    ev.data.fd = fd;
    if (event & 1) {
      ev.events |= EPOLLIN;
    }
    if (event & 2) {
      ev.events |= EPOLLOUT;
    }
    return epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &ev);
  }

  int selector_epoll::mod(int fd, int event) {
    struct epoll_event ev = {};
    ev.data.fd = fd;
    if (event & 1) {
      ev.events |= EPOLLIN;
    }
    if (event & 2) {
      ev.events |= EPOLLOUT;
    }
    return epoll_ctl(fd_, EPOLL_CTL_MOD, fd, &ev);
  }

  int selector_epoll::del(int fd) {
    struct epoll_event ev = {};
    return epoll_ctl(fd_, EPOLL_CTL_DEL, fd, &ev);
  }

  int selector_epoll::select(const struct timespec* timeout) {
    int t = -1;
    if (timeout) {
      t = timeout->tv_sec * 1000 + timeout->tv_nsec / 1000000;
    }
    result_ = epoll_wait(fd_, &buffer_[0], buffer_.size(), t);
    return result_;
  }

  int selector_epoll::event(int i, int& fd, int& event) const {
    if (0 <= i && i < result_) {
      const struct epoll_event& ev = buffer_[i];
      fd = ev.data.fd;
      event = 0;
      if (ev.events & EPOLLIN) {
        event |= 1;
      }
      if (ev.events & EPOLLOUT) {
        event |= 2;
      }
      return 0;
    } else {
      errno = ERANGE;
      return -1;
    }
  }
}
