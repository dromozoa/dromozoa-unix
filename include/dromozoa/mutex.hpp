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

#ifndef DROMOZOA_MUTEX_HPP
#define DROMOZOA_MUTEX_HPP

#include <pthread.h>

namespace dromozoa {
  class mutex {
  public:
    mutex();
    ~mutex();
    void lock();
    void unlock();
    pthread_mutex_t* native_handle();
  private:
    pthread_mutex_t mutex_;
    mutex(const mutex&);
    mutex& operator=(const mutex&);
  };
}

#endif
