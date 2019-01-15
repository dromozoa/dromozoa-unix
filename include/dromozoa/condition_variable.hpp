// Copyright (C) 2018,2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_CONDITION_VARIABLE_HPP
#define DROMOZOA_CONDITION_VARIABLE_HPP

#include <pthread.h>

#include <dromozoa/bind/mutex.hpp>

namespace dromozoa {
  class condition_variable {
  public:
    condition_variable();
    ~condition_variable();
    void notify_one();
    void notify_all();
    void wait(lock_guard<mutex>& lock);
    pthread_cond_t* native_handle();
  private:
    pthread_cond_t cond_;
    condition_variable(const condition_variable&);
    condition_variable& operator=(const condition_variable&);
  };
}

#endif
