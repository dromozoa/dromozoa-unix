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

#include <mach/mach_time.h>

#include <dromozoa/timer_mach_absolute_time.hpp>

namespace dromozoa {
  timer_mach_absolute_time::timer_mach_absolute_time() : start_(), stop_() {}

  timer_mach_absolute_time::~timer_mach_absolute_time() {}

  int timer_mach_absolute_time::start() {
    start_ = mach_absolute_time();
    return 0;
  }

  int timer_mach_absolute_time::stop() {
    stop_ = mach_absolute_time();
    return 0;
  }

  double timer_mach_absolute_time::elapsed() const {
    mach_timebase_info_data_t timebase = {};
    mach_timebase_info(&timebase);
    uint64_t n = (stop_ - start_) * timebase.numer / timebase.denom;
    return n * 0.000000001;
  }
}
