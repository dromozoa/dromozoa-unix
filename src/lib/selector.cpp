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

#include <dromozoa/selector.hpp>

#ifdef HAVE_EPOLL_CREATE1
#include <sys/epoll.h>
#else
#include <fcntl.h>
#endif

namespace dromozoa {
  const int SELECTOR_READ = 1;
  const int SELECTOR_WRITE = 2;

#ifdef HAVE_EPOLL_CREATE1
  const int SELECTOR_CLOEXEC = EPOLL_CLOEXEC;
#else
  const int SELECTOR_CLOEXEC = O_CLOEXEC;
#endif

  selector::~selector() {}
}
