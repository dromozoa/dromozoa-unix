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

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <exception>

#include <dromozoa/bind/unexpected.hpp>

#include <dromozoa/async_service.hpp>
#include <dromozoa/async_task.hpp>
#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/file_descriptor.hpp>

namespace dromozoa {
  namespace {
    class scoped_lock {
    public:
      scoped_lock(pthread_mutex_t* mutex) : mutex_(mutex), locked_() {
        if (int result = pthread_mutex_lock(mutex_)) {
          DROMOZOA_UNEXPECTED(compat_strerror(result));
        } else {
          locked_ = true;
        }
      }

      ~scoped_lock() {
        if (locked_) {
          if (int result = pthread_mutex_unlock(mutex_)) {
            DROMOZOA_UNEXPECTED(compat_strerror(result));
          };
        }
      }

      void wait(pthread_cond_t* condition) {
        if (locked_) {
          if (int result = pthread_cond_wait(condition, mutex_)) {
            DROMOZOA_UNEXPECTED(compat_strerror(result));
          }
        }
      }

    private:
      pthread_mutex_t* mutex_;
      bool locked_;
      scoped_lock(const scoped_lock&);
      scoped_lock& operator=(const scoped_lock&);
    };
  }

  async_service::async_service() : thread_(1) {}

  async_service::~async_service() {}

  int async_service::open() {
    int fd[2] = { -1, -1 };
    if (compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
      return -1;
    }
    file_descriptor fd0(fd[0]);
    file_descriptor fd1(fd[1]);

    if (int result = pthread_cond_init(&condition_, 0)) {
      errno = result;
      return -1;
    }
    if (int result = pthread_mutex_init(&queue_mutex_, 0)) {
      errno = result;
      return -1;
    }
    if (int result = pthread_mutex_init(&ready_mutex_, 0)) {
      errno = result;
      return -1;
    }

    std::vector<pthread_t>::iterator i = thread_.begin();
    std::vector<pthread_t>::iterator end = thread_.end();
    for (; i != end; ++i) {
      if (int result = pthread_create(&*i, 0, &async_service::start_routine, this)) {
        errno = result;
        return -1;
      }
    }

    reader_.swap(fd0);
    writer_.swap(fd1);
    return 0;
  }

  int async_service::close() {
    file_descriptor fd0;
    file_descriptor fd1;
    fd0.swap(reader_);
    fd1.swap(writer_);

    {
      scoped_lock queue_lock(&queue_mutex_);
      queue_.clear();
      queue_.push_back(0);
      if (int result = pthread_cond_broadcast(&condition_)) {
        errno = result;
        return -1;
      }
    }

    std::vector<pthread_t>::const_iterator i = thread_.begin();
    std::vector<pthread_t>::const_iterator end = thread_.end();
    for (; i != end; ++i) {
      if (int result = pthread_join(*i, 0)) {
        errno = result;
        return -1;
      }
    }
    thread_.clear();

    if (fd0.close() == -1) {
      return -1;
    }
    if (fd1.close() == -1) {
      return -1;
    }
    return 0;
  }

  bool async_service::valid() const {
    return reader_.valid();
  }

  int async_service::get() const {
    return reader_.get();
  }

  int async_service::read() const {
    int count = 0;
    char c;
    while (::read(reader_.get(), &c, 1) == 1) {
      ++count;
    }
    return count;
  }

  int async_service::push(async_task* task) {
    scoped_lock queue_lock(&queue_mutex_);
    queue_.push_back(task);
    if (int result = pthread_cond_signal(&condition_)) {
      errno = result;
      return -1;
    }
    return 0;
  }

  async_task* async_service::pop() {
    async_task* task = 0;
    {
      scoped_lock ready_lock(&ready_mutex_);
      if (!ready_.empty()) {
        task = ready_.front();
        ready_.pop_front();
      }
    }
    return task;
  }

  void* async_service::start_routine(void* data) {
    return static_cast<async_service*>(data)->start();
  }

  void* async_service::start() {
    while (true) {
      async_task* task = 0;
      {
        scoped_lock queue_lock(&queue_mutex_);
        if (queue_.empty()) {
          queue_lock.wait(&condition_);
        }
        if (!queue_.empty()) {
          task = queue_.front();
          if (!task) {
            break;
          }
          queue_.pop_front();
        }
      }
      if (task) {
        try {
          task->dispatch();
        } catch (const std::exception& e) {
          DROMOZOA_UNEXPECTED(e.what());
        }
        scoped_lock ready_lock(&ready_mutex_);
        ready_.push_back(task);
        write(writer_.get(), "", 1);
      }
    }
    return 0;
  }
}
