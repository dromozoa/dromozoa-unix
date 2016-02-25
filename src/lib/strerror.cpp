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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <string>

#include <dromozoa/strerror.hpp>

namespace dromozoa {
#ifdef HAVE_STRERROR_R
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
    int save = errno;
    errno = 0;
    std::string result;

    char* buffer = 0;
    size_t size = 32;
    while (true) {
      if (char* b = static_cast<char*>(realloc(buffer, size))) {
        buffer = b;
        if (const char* what = compat_strerror_r(code, buffer, size)) {
          result = what;
        } else if (errno == ERANGE) {
          continue;
        }
      }
      break;
    }
    free(buffer);

    if (result.empty()) {
      std::ostringstream out;
      out << "error number " << code;
      result = out.str();
    }

    errno = save;
    return result;
  }
#else
  std::string compat_strerror(int code) {
    int save = errno;
    errno = 0;
    std::string result;

    if (const char* what = strerror(code)) {
      result = what;
    }

    if (result.empty()) {
      std::ostringstream out;
      out << "error number " << code;
      result = out.str();
    }

    errno = save;
    return result;
  }
#endif
}
