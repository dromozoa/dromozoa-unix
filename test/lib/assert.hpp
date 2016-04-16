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

#ifndef DROMOZOA_ASSERT_HPP
#define DROMOZOA_ASSERT_HPP

#include <assert.h>
#include <fcntl.h>

#define assert_coe(fd) \
  assert("assert_coe" && (fcntl((fd), F_GETFD) & FD_CLOEXEC))

#define assert_ndelay_on(fd) \
  assert("assert_ndelay_on" && (fcntl((fd), F_GETFL) & O_NONBLOCK))

#define assert_ndelay_off(fd) \
  assert("assert_ndelay_off" && !(fcntl((fd), F_GETFL) & O_NONBLOCK))

#endif
