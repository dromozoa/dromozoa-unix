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

#include <fcntl.h>
#include <unistd.h>

#include <exception>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include <dromozoa/bind/unexpected.hpp>

#include <dromozoa/async_service.hpp>
#include <dromozoa/async_task.hpp>
#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/system_error.hpp>

namespace dromozoa {
  namespace {
    class thread {
    public:
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

      void detach() {
        if (int result = pthread_detach(thread_)) {
          throw system_error(result);
        }
        joinable_ = false;
      }

      pthread_t native_handle() {
        return thread_;
      }

      void swap(thread& that) {
        pthread_t thread = thread_;
        thread_ = that.thread_;
        that.thread_ = thread;
        bool joinable = joinable_;
        joinable_ = that.joinable_;
        that.joinable_ = joinable;
      }

    private:
      pthread_t thread_;
      bool joinable_;
      thread& operator=(const thread&);
    };

    class mutex_attr {
    public:
      mutex_attr() : attr_() {
        if (int result = pthread_mutexattr_init(&attr_)) {
          throw system_error(result);
        }
      }

      ~mutex_attr() {
        if (int result = pthread_mutexattr_destroy(&attr_)) {
          throw system_error(result);
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

    class mutex {
    public:
      mutex() : mutex_() {
        if (int result = pthread_mutex_init(&mutex_, mutex_attr().set_error_check().get())) {
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
      conditional_variable() : cond_() {
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

    template <class T>
    class scoped_ptr {
    public:
      explicit scoped_ptr(T* ptr) : ptr_(ptr) {}

      ~scoped_ptr() {
        delete ptr_;
      }

      T* operator->() const {
        return ptr_;
      }

      T* release() {
        T* ptr = ptr_;
        ptr_ = 0;
        return ptr;
      }

    private:
      T* ptr_;
      scoped_ptr(const scoped_ptr&);
      scoped_ptr& operator=(const scoped_ptr&);
    };
  }

  class async_service::impl {
  public:
    typedef std::list<async_task*> queue_type;
    typedef queue_type::iterator queue_iterator;
    typedef std::map<async_task*, queue_iterator> queue_index_type;
    typedef queue_index_type::iterator queue_index_iterator;

    explicit impl(unsigned int max_threads, unsigned int max_spare_threads)
      : max_threads_(max_threads),
        max_spare_threads_(max_spare_threads),
        spare_threads_(),
        current_threads_(),
        current_tasks_() {}

    ~impl() {
      if (valid()) {
        close();
      }
    }

    int open(unsigned int start_threads) {
      int fd[2] = { -1, -1 };
      if (compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
        return -1;
      }
      file_descriptor fd0(fd[0]);
      file_descriptor fd1(fd[1]);

      {
        scoped_lock<mutex> counter_lock(counter_mutex_);
        spare_threads_ += start_threads;
        current_threads_ += start_threads;
        for (unsigned int i = 0; i < start_threads; ++i) {
          thread(&start_routine, this).detach();
        }
      }

      {
        scoped_lock<mutex> ready_lock(ready_mutex_);
        reader_.swap(fd0);
        writer_.swap(fd1);
      }

      return 0;
    }

    int close() {
      file_descriptor fd0;
      file_descriptor fd1;
      {
        scoped_lock<mutex> ready_lock(ready_mutex_);
        fd0.swap(reader_);
        fd1.swap(writer_);
      }

      queue_type queue;
      queue.push_back(0);
      {
        scoped_lock<mutex> queue_lock(queue_mutex_);
        queue.swap(queue_);
        queue_index_.clear();
        queue_condition_.notify_all();
      }

      size_t canceled_tasks = 0;
      {
        queue_iterator i = queue.begin();
        queue_iterator end = queue.end();
        for (; i != end; ++i) {
          async_task* task = *i;
          try {
            task->cancel();
          } catch (const std::exception& e) {
            DROMOZOA_UNEXPECTED(e.what());
          }
        }
        canceled_tasks = queue.size();
        queue.clear();
      }

      {
        scoped_lock<mutex> counter_lock(counter_mutex_);
        current_tasks_ -= canceled_tasks;
        while (current_threads_ > 0) {
          counter_condition_.wait(counter_lock);
        }
      }

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

    void push(async_task* task) {
      {
        scoped_lock<mutex> counter_lock(counter_mutex_);
        ++current_tasks_;
        if (current_threads_ < current_tasks_ && current_threads_ < max_threads_) {
          ++spare_threads_;
          ++current_threads_;
          thread(&start_routine, this).detach();
        }
      }

      {
        scoped_lock<mutex> queue_lock(queue_mutex_);
        queue_iterator i = queue_.insert(queue_.end(), task);
        queue_index_.insert(std::make_pair(task, i));
        queue_condition_.notify_one();
      }
    }

    bool cancel(async_task* task) {
      {
        scoped_lock<mutex> queue_lock(queue_mutex_);
        queue_index_iterator i = queue_index_.find(task);
        if (i == queue_index_.end()) {
          return false;
        }
        queue_.erase(i->second);
        queue_index_.erase(i);
      }

      try {
        task->cancel();
      } catch (const std::exception& e) {
        DROMOZOA_UNEXPECTED(e.what());
      }

      {
        scoped_lock<mutex> counter_lock(counter_mutex_);
        --current_tasks_;
      }

      return true;
    }

    async_task* pop() {
      scoped_lock<mutex> ready_lock(ready_mutex_);
      if (ready_.empty()) {
        return 0;
      } else {
        async_task* task = ready_.front();
        ready_.pop_front();
        return task;
      }
    }

    void info(unsigned int& spare_threads, unsigned int& current_threads, unsigned int& current_tasks) {
      scoped_lock<mutex> counter_lock(counter_mutex_);
      spare_threads = spare_threads_;
      current_threads = current_threads_;
      current_tasks = current_tasks_;
    }

  private:
    unsigned int max_threads_;
    unsigned int max_spare_threads_;
    unsigned int spare_threads_;
    unsigned int current_threads_;
    unsigned int current_tasks_;
    mutex counter_mutex_;
    conditional_variable counter_condition_;

    mutex queue_mutex_;
    queue_type queue_;
    queue_index_type queue_index_;
    conditional_variable queue_condition_;

    mutex ready_mutex_;
    queue_type ready_;
    file_descriptor reader_;
    file_descriptor writer_;

    impl(const impl&);
    impl& operator=(const impl&);

    static void* start_routine(void* self) {
      static_cast<impl*>(self)->start();
      return 0;
    }

    void start() {
      while (true) {
        async_task* task = 0;

        {
          scoped_lock<mutex> queue_lock(queue_mutex_);
          while (queue_.empty()) {
            queue_condition_.wait(queue_lock);
          }
          task = queue_.front();
          if (task) {
            queue_.pop_front();
            queue_index_.erase(task);
          }
        }

        {
          scoped_lock<mutex> counter_lock(counter_mutex_);
          if (task) {
            --spare_threads_;
          } else {
            --spare_threads_;
            --current_threads_;
            counter_condition_.notify_one();
            return;
          }
        }

        try {
          task->dispatch();
        } catch (const std::exception& e) {
          DROMOZOA_UNEXPECTED(e.what());
        }

        {
          scoped_lock<mutex> ready_lock(ready_mutex_);
          ready_.push_back(task);
          if (writer_.valid()) {
            write(writer_.get(), "", 1);
          }
        }

        {
          scoped_lock<mutex> counter_lock(counter_mutex_);
          --current_tasks_;
          if (current_threads_ <= current_tasks_ || spare_threads_ < max_spare_threads_) {
            ++spare_threads_;
          } else {
            --current_threads_;
            counter_condition_.notify_one();
            return;
          }
        }
      }
    }
  };

  async_service::impl* async_service::open(unsigned int start_threads) {
    scoped_ptr<async_service::impl> impl(new async_service::impl(start_threads, start_threads));
    if (impl->open(start_threads) == -1) {
      return 0;
    } else {
      return impl.release();
    }
  }

  async_service::impl* async_service::open(unsigned int start_threads, unsigned int max_threads) {
    scoped_ptr<async_service::impl> impl(new async_service::impl(max_threads, max_threads));
    if (impl->open(start_threads) == -1) {
      return 0;
    } else {
      return impl.release();
    }
  }

  async_service::impl* async_service::open(unsigned int start_threads, unsigned int max_threads, unsigned int max_spare_threads) {
    scoped_ptr<async_service::impl> impl(new async_service::impl(max_threads, max_spare_threads));
    if (impl->open(start_threads) == -1) {
      return 0;
    } else {
      return impl.release();
    }
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

  void async_service::push(async_task* task) {
    impl_->push(task);
  }

  bool async_service::cancel(async_task* task) {
    return impl_->cancel(task);
  }

  async_task* async_service::pop() {
    return impl_->pop();
  }

  void async_service::info(unsigned int& spare_threads, unsigned int& current_threads, unsigned int& current_tasks) {
    impl_->info(spare_threads, current_threads, current_tasks);
  }
}
