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

#include <assert.h>
#include <errno.h>

#include <iostream>

#include <dromozoa/async_service.hpp>
#include <dromozoa/async_task.hpp>
#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/hardware_concurrency.hpp>
#include <dromozoa/system_error.hpp>

void wait(int msec) {
  struct timespec tv = {};
  tv.tv_sec = 0;
  tv.tv_nsec = msec * 1000000;
  if (nanosleep(&tv, 0) == -1) {
    std::cerr << "coult not nanosleep: " << dromozoa::compat_strerror(errno) << std::endl;
  }
}

class async_test_task : public dromozoa::async_task {
public:
  virtual void dispatch() {
    std::cout << "start:" << this << "\n";
    wait(200);
    std::cout << "stop:" << this << "\n";
  }
};

void test1() {
  async_test_task task1;
  async_test_task task2;
  async_test_task task3;

  dromozoa::async_service::impl* impl = dromozoa::async_service::open(1);
  assert(impl);
  dromozoa::async_service service(impl);
  service.push(&task1);
  service.push(&task2);
  service.push(&task3);

  wait(100);
  service.close();

  std::cout << "done\n";
}

int main(int, char*[]) {
  try {
    test1();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
  }
  return 1;
}
