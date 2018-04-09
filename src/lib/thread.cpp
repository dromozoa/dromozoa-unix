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
#include <utility>

#include <dromozoa/system_error.hpp>
#include <dromozoa/thread.hpp>

namespace dromozoa {
  thread::thread(void* (*start_routine)(void*), void* arg) : thread_(), joinable_() {
    if (int result = pthread_create(&thread_, 0, start_routine, arg)) {
      throw system_error(result);
    }
    joinable_ = true;
  }

  thread::~thread() {
    if (joinable_) {
      std::terminate();
    }
  }

  bool thread::joinable() const {
    return joinable_;
  }

  void thread::join() {
    if (int result = pthread_join(thread_, 0)) {
      throw system_error(result);
    }
    joinable_ = false;
  }

  void thread::detach() {
    if (int result = pthread_detach(thread_)) {
      throw system_error(result);
    }
    joinable_ = false;
  }

  pthread_t thread::native_handle() {
    return thread_;
  }

  void thread::swap(thread& that) {
    std::swap(thread_, that.thread_);
    std::swap(joinable_, that.joinable_);
  }
}
