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
#include <stdlib.h>
#include <string.h>

#include <sstream>
#include <string>

#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/errno_saver.hpp>

namespace dromozoa {
  const char* compat_strerror_r_result(const char* result, char*) {
    return result;
  }

  const char* compat_strerror_r_result(int result, char* buffer) {
    if (result == 0) {
      return buffer;
    } else {
      if (result != -1) {
        errno = result;
      }
      return 0;
    }
  }

  const char* compat_strerror_r(int code, char* buffer, size_t size) {
    return compat_strerror_r_result(strerror_r(code, buffer, size), buffer);
  }

  std::string compat_strerror(int code) {
    errno_saver save_errno;
    errno = 0;
    std::string what;

    char* buffer = 0;
    size_t size = 16;
    while (true) {
      size = size * 2;
      if (char* result = static_cast<char*>(realloc(buffer, size))) {
        buffer = result;
        if (const char* result = compat_strerror_r(code, buffer, size)) {
          what = result;
        } else if (errno == ERANGE) {
          continue;
        }
      }
      break;
    }
    free(buffer);

    if (what.empty()) {
      std::ostringstream out;
      out << "error number " << code;
      what = out.str();
    }

    return what;
  }
}
