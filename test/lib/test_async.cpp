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
#include <pthread.h>

#include <iostream>

#include <dromozoa/async_service.hpp>
#include <dromozoa/async_task.hpp>

class nanosleep_task : public dromozoa::async_task {
public:
  explicit nanosleep_task(const struct timespec& tv) : tv_(tv), thread_(), finished_() {}

  virtual void dispatch() {
    thread_ = pthread_self();
    nanosleep(&tv_, 0);
    finished_ = true;
  }

  virtual void cancel() {
    std::cout
        << "  dipatch: " << thread_ << "\n"
        << "  cancel: " << pthread_self() << "\n";
  }

  virtual void result(void*) {
    std::cout
        << "dipatch: " << thread_ << "\n"
        << "result: " << pthread_self() << "\n";
  }

  pthread_t thread() const {
    return thread_;
  }

  bool finished() const {
    return finished_;
  }

private:
  struct timespec tv_;
  pthread_t thread_;
  bool finished_;
  nanosleep_task(const nanosleep_task&);
  nanosleep_task& operator=(const nanosleep_task&);
};

struct timespec make_timespec(time_t s, long n) {
  struct timespec tv = {};
  tv.tv_sec = s;
  tv.tv_nsec = n;
  return tv;
}

void test1() {
  nanosleep_task task1(make_timespec(0, 200000000));
  nanosleep_task task2(make_timespec(0, 200000000));
  nanosleep_task task3(make_timespec(0, 200000000));
  nanosleep_task task4(make_timespec(0, 200000000));

  dromozoa::async_service service(dromozoa::async_service::open(1));
  service.push(&task1);
  service.push(&task2);
  service.push(&task3);
  service.push(&task4);

  struct timespec tv = make_timespec(0, 300000000);
  nanosleep(&tv, 0);
  service.close();

  assert(service.pop() == &task1);
  assert(service.pop() == &task2);
  assert(service.pop() == 0);

  assert(task1.finished());
  assert(task2.finished());
  assert(!task3.finished());
  assert(!task4.finished());

  task1.result(0);
  task2.result(0);
  task3.result(0);
  task4.result(0);

  assert(pthread_equal(task1.thread(), task2.thread()) != 0);
  assert(pthread_equal(task1.thread(), task3.thread()) == 0);
  assert(pthread_equal(task1.thread(), task4.thread()) == 0);
}

void test2() {
  nanosleep_task task1(make_timespec(0, 200000000));
  nanosleep_task task2(make_timespec(0, 200000000));
  nanosleep_task task3(make_timespec(0, 200000000));
  nanosleep_task task4(make_timespec(0, 200000000));

  dromozoa::async_service service(dromozoa::async_service::open(4));
  service.push(&task1);
  service.push(&task2);
  service.push(&task3);
  service.push(&task4);

  struct timespec tv = make_timespec(0, 100000000);
  nanosleep(&tv, 0);
  service.close();

  assert(task1.finished());
  assert(task2.finished());
  assert(task3.finished());
  assert(task4.finished());

  task1.result(0);
  task2.result(0);
  task3.result(0);
  task4.result(0);

  assert(pthread_equal(task1.thread(), task2.thread()) == 0);
  assert(pthread_equal(task1.thread(), task3.thread()) == 0);
  assert(pthread_equal(task1.thread(), task4.thread()) == 0);
}

void test3() {
  nanosleep_task task1(make_timespec(0, 200000000));
  nanosleep_task task2(make_timespec(0, 200000000));
  nanosleep_task task3(make_timespec(0, 200000000));
  nanosleep_task task4(make_timespec(0, 200000000));

  dromozoa::async_service service(dromozoa::async_service::open(1));
  service.push(&task1);
  service.push(&task2);
  service.push(&task3);
  service.push(&task4);

  struct timespec tv = make_timespec(0, 300000000);
  nanosleep(&tv, 0);
  assert(!service.cancel(&task2));
  assert(service.cancel(&task3));
  nanosleep(&tv, 0);
  service.close();

  assert(service.pop() == &task1);
  assert(service.pop() == &task2);
  assert(service.pop() == &task4);
  assert(service.pop() == 0);

  assert(task1.finished());
  assert(task2.finished());
  assert(!task3.finished());
  assert(task4.finished());

  task1.result(0);
  task2.result(0);
  task3.result(0);
  task4.result(0);

  assert(pthread_equal(task1.thread(), task2.thread()) != 0);
  assert(pthread_equal(task1.thread(), task3.thread()) == 0);
  assert(pthread_equal(task1.thread(), task4.thread()) != 0);
}

int main(int, char*[]) {
  try {
    std::cout << "--\n";
    test1();
    std::cout << "--\n";
    test2();
    std::cout << "--\n";
    test3();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
  }
  return 1;
}
