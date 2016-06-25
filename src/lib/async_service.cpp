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
#include <deque>
#include <vector>

#include <dromozoa/bind/unexpected.hpp>

#include <dromozoa/async_service.hpp>
#include <dromozoa/async_task.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/system_error.hpp>

namespace dromozoa {
  namespace {
    class thread {
    public:
      thread() : thread_(), joinable_() {}

      thread(void* (*start_routine)(void*), void* arg) : thread_(), joinable_() {
        if (int result = pthread_create(&thread_, 0, start_routine, arg)) {
          throw system_error(result);
        }
        joinable_ = true;
      }

      ~thread() {
        if (joinable_) {
          std::terminate();
        }
      }

      bool joinable() const {
        return joinable_;
      }

      void join() {
        if (int result = pthread_join(thread_, 0)) {
          throw system_error(result);
        }
        joinable_ = false;
      }

      pthread_t native_handle() {
        return thread_;
      }

      void swap(thread& that) {
        pthread_t thread = thread_;
        bool joinable = joinable_;
        thread_ = that.thread_;
        joinable_ = that.joinable_;
        that.thread_ = thread;
        that.joinable_ = joinable;
      }

    private:
      pthread_t thread_;
      bool joinable_;
      thread(const thread&);
      thread& operator=(const thread&);
    };

    class mutex {
    public:
      mutex() {
        if (int result = pthread_mutex_init(&mutex_, 0)) {
          throw system_error(result);
        }
      }

      ~mutex() {
        if (int result = pthread_mutex_destroy(&mutex_)) {
          throw system_error(result);
        }
      }

      void lock() {
        if (int result = pthread_mutex_lock(&mutex_)) {
          throw system_error(result);
        }
      }

      void unlock() {
        if (int result = pthread_mutex_unlock(&mutex_)) {
          throw system_error(result);
        }
      }

      pthread_mutex_t* native_handle() {
        return &mutex_;
      }

    private:
      pthread_mutex_t mutex_;
      mutex(const mutex&);
      mutex& operator=(const mutex&);
    };

    template <class T>
    class scoped_lock {
    public:
      scoped_lock(T& mutex) : mutex_(&mutex) {
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

    class conditional_variable {
    public:
      conditional_variable() {
        if (int result = pthread_cond_init(&cond_, 0)) {
          throw system_error(result);
        }
      }

      ~conditional_variable() {
        if (int result = pthread_cond_destroy(&cond_)) {
          throw system_error(result);
        }
      }

      void notify_one() {
        if (int result = pthread_cond_signal(&cond_)) {
          throw system_error(result);
        }
      }

      void notify_all() {
        if (int result = pthread_cond_broadcast(&cond_)) {
          throw system_error(result);
        }
      }

      void wait(scoped_lock<mutex>& lock) {
        if (int result = pthread_cond_wait(&cond_, lock.mutex()->native_handle())) {
          throw system_error(result);
        }
      }

      pthread_cond_t* native_handle() {
        return &cond_;
      }

    private:
      pthread_cond_t cond_;
    };
  }

  class async_service::impl {
  public:
    static void* start_routine(void* self) {
      return static_cast<impl*>(self)->start();
    }

    explicit impl() {}

    ~impl() {}

    int open(size_t size) {
      int fd[2] = { -1, -1 };
      if (compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
        return -1;
      }
      file_descriptor fd0(fd[0]);
      file_descriptor fd1(fd[1]);

      std::vector<thread> thread_pool(size);
      std::vector<thread>::iterator i = thread_pool.begin();
      std::vector<thread>::iterator end = thread_pool.end();
      for (; i != end; ++i) {
        thread(&start_routine, this).swap(*i);
      }

      reader_.swap(fd0);
      writer_.swap(fd1);
      thread_pool_.swap(thread_pool);
      return 0;
    }

    int close() {
      file_descriptor fd0;
      file_descriptor fd1;
      fd0.swap(reader_);
      fd1.swap(writer_);

      {
        scoped_lock<mutex> queue_lock(queue_mutex_);
        queue_.clear();
        queue_.push_back(0);
        condition_.notify_all();
      }

      std::vector<thread>::iterator i = thread_pool_.begin();
      std::vector<thread>::iterator end = thread_pool_.end();
      for (; i != end; ++i) {
        i->join();
      }
      thread_pool_.clear();

      if (fd0.close() == -1) {
        return -1;
      }
      if (fd1.close() == -1) {
        return -1;
      }
      return 0;
    }

    bool valid() const {
      return reader_.valid();
    }

    int get() const {
      return reader_.get();
    }

    int read() const {
      int count = 0;
      char c;
      while (::read(reader_.get(), &c, 1) == 1) {
        ++count;
      }
      return count;
    }

    int push(async_task* task) {
      scoped_lock<mutex> queue_lock(queue_mutex_);
      queue_.push_back(task);
      condition_.notify_one();
      return 0;
    }

    async_task* pop() {
      async_task* task = 0;
      {
        scoped_lock<mutex> ready_lock(ready_mutex_);
        if (!ready_.empty()) {
          task = ready_.front();
          ready_.pop_front();
        }
      }
      return task;
    }

    void* start() {
      while (true) {
        async_task* task = 0;
        {
          scoped_lock<mutex> queue_lock(queue_mutex_);
          if (queue_.empty()) {
            condition_.wait(queue_lock);
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
          scoped_lock<mutex> ready_lock(ready_mutex_);
          ready_.push_back(task);
          write(writer_.get(), "", 1);
        }
      }
      return 0;
    }

  private:
    file_descriptor reader_;
    file_descriptor writer_;
    std::vector<thread> thread_pool_;
    conditional_variable condition_;
    std::deque<async_task*> queue_;
    mutex queue_mutex_;
    std::deque<async_task*> ready_;
    mutex ready_mutex_;
  };

  async_service::impl* async_service::open(size_t size) {
    async_service::impl* impl = new async_service::impl();
    if (impl->open(size) == -1) {
      delete impl;
      return 0;
    }
    return impl;
  }

  async_service::async_service(impl* impl) : impl_(impl) {}

  async_service::~async_service() {
    delete impl_;
  }

  int async_service::close() {
    return impl_->close();
  }

  bool async_service::valid() const {
    return impl_->valid();
  }

  int async_service::get() const {
    return impl_->get();
  }

  int async_service::read() const {
    return impl_->read();
  }

  int async_service::push(async_task* task) {
    return impl_->push(task);
  }

  async_task* async_service::pop() {
    return impl_->pop();
  }
}
