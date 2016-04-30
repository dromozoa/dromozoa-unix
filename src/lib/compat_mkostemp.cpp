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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <dromozoa/bind/unexpected.hpp>

#include <dromozoa/coe.hpp>
#include <dromozoa/compat_mkostemp.hpp>
#include <dromozoa/compat_strerror.hpp>
#include <dromozoa/errno_saver.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/ndelay.hpp>
#include <dromozoa/sigmask.hpp>

namespace dromozoa {
#ifdef HAVE_MKOSTEMP
  int compat_mkostemp(char* tmpl, int flags) {
    return mkostemp(tmpl, flags);
  }
#else
  namespace {
    class scoped_unlink {
    public:
      explicit scoped_unlink(const char* path) : path_(path) {}

      ~scoped_unlink() {
        if (path_) {
          errno_saver save;
          if (unlink(path_) == -1) {
            DROMOZOA_UNEXPECTED(compat_strerror(errno));
          }
        }
      }

      const char* release() {
        const char* path = path_;
        path_ = 0;
        return path;
      }

    private:
      const char* path_;
      scoped_unlink(const scoped_unlink&);
      scoped_unlink& operator=(const scoped_unlink&);
    };
  }

  int compat_mkostemp(char* tmpl, int flags) {
    sigset_t mask;
    if (sigmask_block_all_signals(&mask)) {
      return -1;
    }
    sigmask_saver save_mask(mask);

    file_descriptor fd(mkstemp(tmpl));
    if (!fd.valid()) {
      return -1;
    }
    scoped_unlink scoped(tmpl);

    if (flags & O_CLOEXEC) {
      if (coe(fd.get()) == -1) {
        return -1;
      }
    }

    flags &= O_APPEND | O_SYNC;
    if (flags) {
      int result = fcntl(fd.get(), F_GETFL);
      if (result == -1) {
        return -1;
      }
      if (fcntl(fd.get(), F_SETFL, result | flags) == -1) {
        return -1;
      }
    }

    scoped.release();
    return fd.release();
  }
#endif
}
