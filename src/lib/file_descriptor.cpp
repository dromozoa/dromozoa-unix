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

#include <errno.h>
#include <unistd.h>

#include <utility>

#include <dromozoa/bind/unexpected.hpp>

#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/errno_saver.hpp>
#include <dromozoa/file_descriptor.hpp>

namespace dromozoa {
  file_descriptor::file_descriptor() : fd_(-1) {}

  file_descriptor::file_descriptor(int fd) : fd_(fd) {}

  file_descriptor::~file_descriptor() {
    if (fd_ != -1) {
      errno_saver save_errno;
      if (close() == -1) {
        DROMOZOA_UNEXPECTED(compat_strerror(errno));
      }
    }
  }

  int file_descriptor::close() {
    int fd = fd_;
    fd_ = -1;
    return ::close(fd);
  }

  int file_descriptor::release() {
    int fd = fd_;
    fd_ = -1;
    return fd;
  }

  bool file_descriptor::valid() const {
    return fd_ != -1;
  }

  int file_descriptor::get() const {
    return fd_;
  }

  void file_descriptor::swap(file_descriptor& that) {
    std::swap(fd_, that.fd_);
  }
}
