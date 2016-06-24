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

#ifndef DROMOZOA_ASYNC_SERVICE_HPP
#define DROMOZOA_ASYNC_SERVICE_HPP

#include <pthread.h>

#include <deque>
#include <vector>

#include <dromozoa/file_descriptor.hpp>

namespace dromozoa {
  class async_task;

  class async_service {
  public:
    async_service();
    ~async_service();
    int open();
    int close();
    bool valid() const;
    int get() const;
    int read() const;
    int push(async_task* task);
    async_task* pop();
  private:
    file_descriptor reader_;
    file_descriptor writer_;

    std::vector<pthread_t> thread_;
    pthread_cond_t condition_;
    std::deque<async_task*> queue_;
    pthread_mutex_t queue_mutex_;
    std::deque<async_task*> ready_;
    pthread_mutex_t ready_mutex_;

    async_service(const async_service&);
    async_service& operator=(const async_service&);
    static void* start_routine(void* data);
    void* start();
  };
}

#endif
