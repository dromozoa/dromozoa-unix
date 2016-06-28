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

#include <errno.h>
#include <unistd.h>

#ifdef HAVE_SYSCTLBYNAME
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include <dromozoa/hardware_concurrency.hpp>

namespace dromozoa {
#ifdef HAVE_SYSCTLBYNAME
  unsigned int hardware_concurrency() {
    int result = 0;
    size_t size = sizeof(result);
    if (sysctlbyname("hw.logicalcpu", &result, &size, 0, 0) == 0 && result > 0) {
      return result;
    } else {
      return 0;
    }
  }
#else
  unsigned int hardware_concurrency() {
    errno = 0;
    int result = sysconf(_SC_NPROCESSORS_ONLN);
    if (errno == 0 && result > 0) {
      return result;
    } else {
      return 0;
    }
  }
#endif
}
