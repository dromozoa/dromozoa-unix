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

#include <dromozoa/mutex.hpp>
#include <dromozoa/system_error.hpp>

namespace dromozoa {
  namespace {
    class mutex_attr {
    public:
      mutex_attr() : attr_() {
        if (int result = pthread_mutexattr_init(&attr_)) {
          throw system_error(result);
        }
      }

      ~mutex_attr() {
        if (pthread_mutexattr_destroy(&attr_)) {
          std::terminate();
        }
      }

      mutex_attr& set_error_check() {
        return set_type(PTHREAD_MUTEX_ERRORCHECK);
      }

      pthread_mutexattr_t* get() {
        return &attr_;
      }

    private:
      pthread_mutexattr_t attr_;
      mutex_attr(const mutex_attr&);
      mutex_attr& operator=(const mutex_attr&);

      mutex_attr& set_type(int kind) {
        if (int result = pthread_mutexattr_settype(&attr_, kind)) {
          throw system_error(result);
        }
        return *this;
      }
    };
  }

  mutex::mutex() : mutex_() {
    if (int result = pthread_mutex_init(&mutex_, mutex_attr().set_error_check().get())) {
      throw system_error(result);
    }
  }

  mutex::~mutex() {
    if (pthread_mutex_destroy(&mutex_)) {
      std::terminate();
    }
  }

  void mutex::lock() {
    if (int result = pthread_mutex_lock(&mutex_)) {
      throw system_error(result);
    }
  }

  void mutex::unlock() {
    if (int result = pthread_mutex_unlock(&mutex_)) {
      throw system_error(result);
    }
  }

  pthread_mutex_t* mutex::native_handle() {
    return &mutex_;
  }
}
