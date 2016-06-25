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

#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/system_error.hpp>

namespace dromozoa {
  system_error::system_error(int code) : code_(code) {}

  system_error::~system_error() throw() {}

  const char* system_error::what() const throw() {
    try {
      if (what_.empty()) {
        what_ = compat_strerror(code_);
      }
      return what_.c_str();
    } catch (const std::exception e) {
      return e.what();
    }
  }

  int system_error::code() const {
    return code_;
  }
}
