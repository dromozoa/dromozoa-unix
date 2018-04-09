// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <pthread.h>

#include <exception>

#include <dromozoa/condition_variable.hpp>
#include <dromozoa/system_error.hpp>

namespace dromozoa {
  condition_variable::condition_variable() : cond_() {
    if (int result = pthread_cond_init(&cond_, 0)) {
      throw system_error(result);
    }
  }

  condition_variable::~condition_variable() {
    if (pthread_cond_destroy(&cond_)) {
      std::terminate();
    }
  }

  void condition_variable::notify_one() {
    if (int result = pthread_cond_signal(&cond_)) {
      throw system_error(result);
    }
  }

  void condition_variable::notify_all() {
    if (int result = pthread_cond_broadcast(&cond_)) {
      throw system_error(result);
    }
  }

  void condition_variable::wait(scoped_lock<mutex>& lock) {
    if (int result = pthread_cond_wait(&cond_, lock.mutex()->native_handle())) {
      throw system_error(result);
    }
  }

  pthread_cond_t* condition_variable::native_handle() {
    return &cond_;
  }
}
