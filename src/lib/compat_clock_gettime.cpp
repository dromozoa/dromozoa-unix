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
#include <limits.h>
#include <stdint.h>
#include <sys/time.h>

#ifdef HAVE_MACH_ABSOLUTE_TIME
#include <mach/mach_time.h>
#endif

#include <dromozoa/compat_clock_gettime.hpp>

namespace dromozoa {
#ifdef HAVE_CLOCK_GETTIME
#if INT_MIN <= CLOCK_REALTIME && CLOCK_REALTIME <= INT_MAX
  const int COMPAT_CLOCK_REALTIME = CLOCK_REALTIME;
#endif
#if INT_MIN <= CLOCK_MONOTONIC && CLOCK_MONOTONIC <= INT_MAX
  const int COMPAT_CLOCK_MONOTONIC = CLOCK_MONOTONIC;
#endif
#ifdef CLOCK_MONOTONIC_RAW
#if INT_MIN <= CLOCK_MONOTONIC_RAW && CLOCK_MONOTONIC_RAW <= INT_MAX
  const int COMPAT_CLOCK_MONOTONIC_RAW = CLOCK_MONOTONIC_RAW;
#endif
#else
  const int COMPAT_CLOCK_MONOTONIC_RAW = COMPAT_CLOCK_MONOTONIC;
#endif
#else
  const int COMPAT_CLOCK_REALTIME = 0;
  const int COMPAT_CLOCK_MONOTONIC = 1;
  const int COMPAT_CLOCK_MONOTONIC_RAW = COMPAT_CLOCK_MONOTONIC;
#endif

#ifdef HAVE_CLOCK_GETTIME
  int compat_clock_gettime(int clock_id, struct timespec* tp) {
    return clock_gettime(static_cast<clockid_t>(clock_id), tp);
  }
#else
  namespace {
    int clock_gettime_realtime(struct timespec* tp) {
      struct timeval tv = {};
      if (gettimeofday(&tv, 0) == -1) {
        return -1;
      } else {
        tp->tv_sec = tv.tv_sec;
        tp->tv_nsec = tv.tv_usec * 1000;
        return 0;
      }
    }

#ifdef HAVE_MACH_ABSOLUTE_TIME
    int clock_gettime_monotonic(struct timespec* tp) {
      uint64_t t = mach_absolute_time();
      mach_timebase_info_data_t timebase = {};
      mach_timebase_info(&timebase);
      t *= timebase.numer;
      t /= timebase.denom;
      tp->tv_sec = t / 1000000000;
      tp->tv_nsec = t % 1000000000;
      return 0;
    }
#endif
  }

  int compat_clock_gettime(int clock_id, struct timespec* tp) {
    switch (clock_id) {
      case COMPAT_CLOCK_REALTIME:
        return clock_gettime_realtime(tp);
      case COMPAT_CLOCK_MONOTONIC:
        return clock_gettime_monotonic(tp);
      default:
        errno = EINVAL;
        return -1;
    }
  }
#endif
}
