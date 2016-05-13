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

#include <iomanip>
#include <iostream>

#include <dromozoa/compat_clock_gettime.hpp>

#include "assert.hpp"

int main(int, char*[]) {
  std::cout << std::setfill('0');

  struct timespec tv1 = {};
  assert(dromozoa::compat_clock_gettime(dromozoa::COMPAT_CLOCK_REALTIME, &tv1) == 0);
  std::cout << tv1.tv_sec << "." << std::setw(9) << tv1.tv_nsec << "\n";

  struct timespec tv2 = {};
  assert(dromozoa::compat_clock_gettime(dromozoa::COMPAT_CLOCK_MONOTONIC, &tv2) == 0);
  std::cout << tv2.tv_sec << "." << std::setw(9) << tv2.tv_nsec << "\n";

  struct timespec tv3 = {};
  assert(dromozoa::compat_clock_gettime(dromozoa::COMPAT_CLOCK_MONOTONIC, &tv3) == 0);
  std::cout << tv3.tv_sec << "." << std::setw(9) << tv3.tv_nsec << "\n";

  struct timespec tv4 = {};
  assert(dromozoa::compat_clock_gettime(dromozoa::COMPAT_CLOCK_MONOTONIC_RAW, &tv4) == 0);
  std::cout << tv4.tv_sec << "." << std::setw(9) << tv4.tv_nsec << "\n";

  tv3.tv_sec -= tv2.tv_sec;
  tv3.tv_nsec -= tv2.tv_nsec;
  if (tv3.tv_nsec < 0) {
    --tv3.tv_sec;
    tv3.tv_nsec += 1000000000;
  }
  std::cout << tv3.tv_sec << "." << std::setw(9) << tv3.tv_nsec << "\n";
  assert(tv3.tv_sec == 0);
  assert(0 <= tv3.tv_nsec && tv3.tv_nsec <= 999999999);

  return 0;
}
