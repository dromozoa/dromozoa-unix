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

#ifndef DROMOZOA_TIMER_CLOCK_GETTIME_HPP
#define DROMOZOA_TIMER_CLOCK_GETTIME_HPP

#include <time.h>

#include <dromozoa/timer.hpp>

namespace dromozoa {
  class timer_clock_gettime : public timer {
  public:
    timer_clock_gettime();
    virtual ~timer_clock_gettime();
    virtual int start();
    virtual int stop();
    virtual double elapsed() const;
  private:
    struct timespec start_;
    struct timespec stop_;
    timer_clock_gettime(const timer_clock_gettime&);
    timer_clock_gettime& operator=(const timer_clock_gettime&);
  };
}

#endif
