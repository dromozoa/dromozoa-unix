// Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <dromozoa/async_task.hpp>

namespace dromozoa {
  class async_service {
  public:
    class impl;
    static impl* open(unsigned int start_threads);
    static impl* open(unsigned int start_threads, unsigned int max_threads);
    static impl* open(unsigned int start_threads, unsigned int max_threads, unsigned int max_spare_threads);
    async_service(impl* impl);
    ~async_service();
    int close();
    bool valid() const;
    int get() const;
    int read() const;
    void push(async_task* task);
    bool cancel(async_task* task);
    async_task* pop();
    void info(unsigned int& spare_threads, unsigned int& current_threads, unsigned int& current_tasks);
  private:
    impl* impl_;
    async_service(const async_service&);
    async_service& operator=(const async_service&);
  };
}

#endif
