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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>

#include <dromozoa/selector.hpp>

#if defined(HAVE_EPOLL_CREATE) || defined(HAVE_EPOLL_CREATE1)
#include <dromozoa/selector_epoll.hpp>
#elif defined(HAVE_KQUEUE)
#include <dromozoa/selector_kqueue.hpp>
#endif

namespace dromozoa {
  const int SELECTOR_READ = 1;
  const int SELECTOR_WRITE = 2;

#ifdef EPOLL_CLOEXEC
  const int SELECTOR_CLOEXEC = EPOLL_CLOEXEC;
#else
  const int SELECTOR_CLOEXEC = O_CLOEXEC;
#endif

  selector_impl::~selector_impl() {}

  selector_impl* selector::open(int flags) {
#if defined(HAVE_EPOLL_CREATE) || defined(HAVE_EPOLL_CREATE1)
    typedef selector_epoll selector_impl_type;
#elif defined(HAVE_KQUEUE)
    typedef selector_kqueue selector_impl_type;
#endif
    scoped_ptr<selector_impl> impl(new selector_impl_type());
    if (impl->open(flags) == -1) {
      return 0;
    } else {
      return impl.release();
    }
  }

  selector::selector(selector_impl* impl) : impl_(impl) {}

  selector::~selector() {}

  int selector::close() {
    return impl_->close();
  }

  bool selector::valid() const {
    return impl_->valid();
  }

  int selector::get() const {
    return impl_->get();
  }

  int selector::add(int fd, int event) {
    return impl_->add(fd, event);
  }

  int selector::mod(int fd, int event) {
    return impl_->mod(fd, event);
  }

  int selector::del(int fd) {
    return impl_->del(fd);
  }

  int selector::select(const struct timespec* timeout) {
    return impl_->select(timeout);
  }

  int selector::event(int i, int& fd, int& event) const {
    return impl_->event(i, fd, event);
  }
}
