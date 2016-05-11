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

#include <dromozoa/timer_clock_gettime.hpp>

namespace dromozoa {
  namespace {
#ifdef CLOCK_MONOTONIC_RAW
    static const clockid_t compat_clock_monotonic = CLOCK_MONOTONIC_RAW;
#else
    static const clockid_t compat_clock_monotonic = CLOCK_MONOTONIC;
#endif
  }

  timer_clock_gettime::timer_clock_gettime() : start_(), stop_() {}

  timer_clock_gettime::~timer_clock_gettime() {}

  int timer_clock_gettime::start() {
    return clock_gettime(compat_clock_monotonic, &start_);
  }

  int timer_clock_gettime::stop() {
    return clock_gettime(compat_clock_monotonic, &stop_);
  }

  double timer_clock_gettime::elapsed() const {
    time_t s = stop_.tv_sec - start_.tv_sec;
    long n = stop_.tv_nsec - start_.tv_nsec;
    if (n < 0) {
      --s;
      n += 1000000000;
    }
    return s + n * 0.000000001;
  }
}
