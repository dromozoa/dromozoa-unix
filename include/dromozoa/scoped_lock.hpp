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

#ifndef DROMOZOA_SCOPED_LOCK_HPP
#define DROMOZOA_SCOPED_LOCK_HPP

#include <dromozoa/mutex.hpp>

namespace dromozoa {
  template <class T = mutex>
  class scoped_lock {
  public:
    explicit scoped_lock(T& mutex) : mutex_(&mutex) {
      mutex_->lock();
    }

    ~scoped_lock() {
      mutex_->unlock();
    }

    T* mutex() const {
      return mutex_;
    }

  private:
    T* mutex_;
    scoped_lock(const scoped_lock&);
    scoped_lock& operator=(const scoped_lock&);
  };
}

#endif
