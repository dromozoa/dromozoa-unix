// Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include <list>
#include <map>
#include <utility>

#include <dromozoa/bind/unexpected.hpp>

#include <dromozoa/async_service.hpp>
#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/condition_variable.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/mutex.hpp>
#include <dromozoa/scoped_lock.hpp>
#include <dromozoa/sigmask.hpp>
#include <dromozoa/thread.hpp>

namespace dromozoa {
  async_service_task::~async_service_task() {}

  class async_service_impl {
  public:
    typedef std::list<async_service_task*> queue_type;
    typedef queue_type::iterator queue_iterator;
    typedef std::map<async_service_task*, queue_iterator> queue_index_type;
    typedef queue_index_type::iterator queue_index_iterator;

    explicit async_service_impl(unsigned int max_threads, unsigned int max_spare_threads)
      : max_threads_(max_threads),
        max_spare_threads_(max_spare_threads),
        spare_threads_(),
        current_threads_(),
        current_tasks_() {}

    ~async_service_impl() {
      if (valid()) {
        close();
      }
    }

    int open(unsigned int start_threads) {
      int fd[2] = { -1, -1 };
      if (compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
        return -1;
      }
      file_descriptor reader(fd[0]);
      file_descriptor writer(fd[1]);

      if (start_threads > 0) {
        scoped_lock<> counter_lock(counter_mutex_);

        sigset_t mask;
        if (sigmask_block_all_signals(&mask)) {
          return -1;
        }
        sigmask_saver save_mask(mask);

        spare_threads_ += start_threads;
        current_threads_ += start_threads;
        for (unsigned int i = 0; i < start_threads; ++i) {
          thread(&start_routine, this).detach();
        }
      }

      {
        scoped_lock<> ready_lock(ready_mutex_);
        ready_reader_.swap(reader);
        ready_writer_.swap(writer);
      }

      return 0;
    }

    int close() {
      file_descriptor reader;
      file_descriptor writer;
      {
        scoped_lock<> ready_lock(ready_mutex_);
        reader.swap(ready_reader_);
        writer.swap(ready_writer_);
      }

      queue_type queue;
      queue.push_back(0);
      {
        scoped_lock<> queue_lock(queue_mutex_);
        queue.swap(queue_);
        queue_index_.clear();
        queue_condition_.notify_all();
      }

      size_t canceled_tasks = 0;
      {
        queue_iterator i = queue.begin();
        queue_iterator end = queue.end();
        for (; i != end; ++i) {
          async_service_task* task = *i;
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
        scoped_lock<> counter_lock(counter_mutex_);
        current_tasks_ -= canceled_tasks;
        while (current_threads_ > 0) {
          counter_condition_.wait(counter_lock);
        }
      }

      if (reader.close() == -1) {
        return -1;
      }
      if (writer.close() == -1) {
        return -1;
      }
      return 0;
    }

    bool valid() const {
      return ready_reader_.valid();
    }

    int get() const {
      return ready_reader_.get();
    }

    int read() const {
      int count = 0;
      char c;
      while (::read(ready_reader_.get(), &c, 1) == 1) {
        ++count;
      }
      return count;
    }

    int push(async_service_task* task) {
      {
        scoped_lock<> counter_lock(counter_mutex_);
        ++current_tasks_;
        if (current_threads_ < current_tasks_ && current_threads_ < max_threads_) {
          sigset_t mask;
          if (sigmask_block_all_signals(&mask)) {
            return -1;
          }
          sigmask_saver save_mask(mask);

          ++spare_threads_;
          ++current_threads_;
          thread(&start_routine, this).detach();
        }
      }

      {
        scoped_lock<> queue_lock(queue_mutex_);
        queue_iterator i = queue_.insert(queue_.end(), task);
        queue_index_.insert(std::make_pair(task, i));
        queue_condition_.notify_one();
      }
      return 0;
    }

    bool cancel(async_service_task* task) {
      {
        scoped_lock<> queue_lock(queue_mutex_);
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
        scoped_lock<> counter_lock(counter_mutex_);
        --current_tasks_;
      }

      return true;
    }

    async_service_task* pop() {
      scoped_lock<> ready_lock(ready_mutex_);
      if (ready_.empty()) {
        return 0;
      } else {
        async_service_task* task = ready_.front();
        ready_.pop_front();
        return task;
      }
    }

    void info(unsigned int& spare_threads, unsigned int& current_threads, unsigned int& current_tasks) {
      scoped_lock<> counter_lock(counter_mutex_);
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

    mutex              counter_mutex_;
    condition_variable counter_condition_;
    mutex              queue_mutex_;
    queue_type         queue_;
    queue_index_type   queue_index_;
    condition_variable queue_condition_;
    mutex              ready_mutex_;
    queue_type         ready_;
    file_descriptor    ready_reader_;
    file_descriptor    ready_writer_;

    async_service_impl(const async_service_impl&);
    async_service_impl& operator=(const async_service_impl&);

    static void* start_routine(void* self) {
      static_cast<async_service_impl*>(self)->start();
      return 0;
    }

    void start() {
      while (true) {
        async_service_task* task = 0;

        {
          scoped_lock<> queue_lock(queue_mutex_);
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
          scoped_lock<> counter_lock(counter_mutex_);
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
          scoped_lock<> ready_lock(ready_mutex_);
          ready_.push_back(task);
          if (ready_writer_.valid()) {
            write(ready_writer_.get(), "", 1);
          }
        }

        {
          scoped_lock<> counter_lock(counter_mutex_);
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

  async_service_impl* async_service::open(unsigned int start_threads) {
    scoped_ptr<async_service_impl> impl(new async_service_impl(start_threads, start_threads));
    if (impl->open(start_threads) == -1) {
      return 0;
    } else {
      return impl.release();
    }
  }

  async_service_impl* async_service::open(unsigned int start_threads, unsigned int max_threads) {
    scoped_ptr<async_service_impl> impl(new async_service_impl(max_threads, max_threads));
    if (impl->open(start_threads) == -1) {
      return 0;
    } else {
      return impl.release();
    }
  }

  async_service_impl* async_service::open(unsigned int start_threads, unsigned int max_threads, unsigned int max_spare_threads) {
    scoped_ptr<async_service_impl> impl(new async_service_impl(max_threads, max_spare_threads));
    if (impl->open(start_threads) == -1) {
      return 0;
    } else {
      return impl.release();
    }
  }

  async_service::async_service(async_service_impl* impl) : impl_(impl) {}

  async_service::~async_service() {}

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

  int async_service::push(async_service_task* task) {
    return impl_->push(task);
  }

  bool async_service::cancel(async_service_task* task) {
    return impl_->cancel(task);
  }

  async_service_task* async_service::pop() {
    return impl_->pop();
  }

  void async_service::info(unsigned int& spare_threads, unsigned int& current_threads, unsigned int& current_tasks) {
    impl_->info(spare_threads, current_threads, current_tasks);
  }
}
