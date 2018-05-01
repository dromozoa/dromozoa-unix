// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include <signal.h>
#include <stddef.h>
#include <unistd.h>

#include <exception>
#include <set>

#include <dromozoa/bind/unexpected.hpp>

#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/mutex.hpp>
#include <dromozoa/ndelay.hpp>
#include <dromozoa/scoped_lock.hpp>
#include <dromozoa/selfpipe.hpp>
#include <dromozoa/sigmask.hpp>
#include <dromozoa/system_error.hpp>
#include <dromozoa/thread.hpp>

namespace dromozoa {
  namespace {
    scoped_ptr<thread> selfpipe_thread;
    mutex              selfpipe_mutex;
    int                selfpipe_reader = -1;
    int                selfpipe_writer = -1;
    struct sigaction   selfpipe_save_sa = {};
    mutex              selfpipe_subscribers_mutex;
    std::set<int>      selfpipe_subscribers;
  }
}

extern "C" void dromozoa_selfpipe_trigger(int) {
  write(dromozoa::selfpipe_writer, "", 1);
}

namespace dromozoa {
  namespace {
    void* selfpipe_publisher(void*) {
      while (true) {
        char c;
        ssize_t result = read(selfpipe_reader, &c, 1);
        if (result == 1) {
          scoped_lock<> subscribers_lock(selfpipe_subscribers_mutex);
          std::set<int>::const_iterator i = selfpipe_subscribers.begin();
          std::set<int>::const_iterator end = selfpipe_subscribers.end();
          for (; i != end; ++i) {
            write(*i, "", 1);
          }
        } else {
          if (result == -1) {
            if (errno == EINTR) {
              continue;
            } else {
              DROMOZOA_UNEXPECTED(compat_strerror(errno));
            }
          }
          return 0;
        }
      }
    }

    int selfpipe_open(int subscriber) {
      scoped_lock<> lock(selfpipe_mutex);

      sigset_t mask;
      if (sigmask_block_all_signals(&mask)) {
        return -1;
      }
      sigmask_saver save_mask(mask);

      {
        scoped_lock<> subscribers_lock(selfpipe_subscribers_mutex);
        std::set<int> subscribers = selfpipe_subscribers;
        if (!subscribers.insert(subscriber).second) {
          errno = EINVAL;
          return -1;
        }

        if (!selfpipe_thread) {
          int fd[2] = { -1, -1 };
          if (compat_pipe2(fd, O_CLOEXEC) == -1) {
            return -1;
          }
          file_descriptor reader(fd[0]);
          file_descriptor writer(fd[1]);

          if (ndelay_on(writer.get()) == -1) {
            return -1;
          }

          selfpipe_reader = reader.get();
          selfpipe_writer = writer.get();

          struct sigaction sa = {};
          sa.sa_handler = dromozoa_selfpipe_trigger;
          if (sigaction(SIGCHLD, &sa, &selfpipe_save_sa) == -1) {
            selfpipe_reader = -1;
            selfpipe_writer = -1;
            return -1;
          }

          try {
            selfpipe_thread.reset(new thread(&selfpipe_publisher, 0));
          } catch (...) {
            if (sigaction(SIGCHLD, &selfpipe_save_sa, 0) == -1) {
              DROMOZOA_UNEXPECTED(compat_strerror(errno));
            }
            selfpipe_reader = -1;
            selfpipe_writer = -1;
            throw;
          }

          reader.release();
          writer.release();
        }

        selfpipe_subscribers.swap(subscribers);
      }

      return 0;
    }

    int selfpipe_close(int subscriber) {
      scoped_lock<> lock(selfpipe_mutex);

      sigset_t mask;
      if (sigmask_block_all_signals(&mask)) {
        return -1;
      }
      sigmask_saver save_mask(mask);

      {
        scoped_lock<> subscribers_lock(selfpipe_subscribers_mutex);
        if (selfpipe_subscribers.erase(subscriber) != 1) {
          errno = EINVAL;
          return -1;
        }
        if (!selfpipe_subscribers.empty()) {
          return 0;
        }
      }

      if (sigaction(SIGCHLD, &selfpipe_save_sa, 0) == -1) {
        DROMOZOA_UNEXPECTED(compat_strerror(errno));
      }

      file_descriptor writer(selfpipe_writer);
      selfpipe_writer = -1;
      if (writer.close() == -1) {
        DROMOZOA_UNEXPECTED(compat_strerror(errno));
      }

      try {
        selfpipe_thread->join();
        selfpipe_thread.reset();
      } catch (const std::exception& e) {
        DROMOZOA_UNEXPECTED(e.what());
        std::terminate();
      } catch (...) {
        std::terminate();
      }

      file_descriptor reader(selfpipe_reader);
      selfpipe_reader = -1;
      if (reader.close() == -1) {
        DROMOZOA_UNEXPECTED(compat_strerror(errno));
      }

      return 0;
    }
  }

  class selfpipe_impl {
  public:
    ~selfpipe_impl() {
      if (valid()) {
        close();
      }
    }

    int open() {
      int fd[2] = { -1, -1 };
      if (compat_pipe2(fd, O_CLOEXEC | O_NONBLOCK) == -1) {
        return -1;
      }
      file_descriptor reader(fd[0]);
      file_descriptor writer(fd[1]);

      if (selfpipe_open(writer.get()) == -1) {
        return -1;
      }

      reader_.swap(reader);
      writer_.swap(writer);
      return 0;
    }

    int close() {
      file_descriptor reader;
      file_descriptor writer;

      reader.swap(reader_);
      writer.swap(writer_);

      if (selfpipe_close(writer.get()) == -1) {
        return -1;
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

  private:
    file_descriptor reader_;
    file_descriptor writer_;
  };

  selfpipe_impl* selfpipe::open() {
    scoped_ptr<selfpipe_impl> impl(new selfpipe_impl());
    if (impl->open() == -1) {
      return 0;
    } else {
      return impl.release();
    }
  }

  selfpipe::selfpipe(selfpipe_impl* impl) : impl_(impl) {}

  selfpipe::~selfpipe() {}

  int selfpipe::close() {
    return impl_->close();
  }

  bool selfpipe::valiid() const {
    return impl_->valid();
  }

  int selfpipe::get() const {
    return impl_->get();
  }

  int selfpipe::read() const {
    return impl_->read();
  }
}
