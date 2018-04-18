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

#ifndef DROMOZOA_SCOPED_PTR_HPP
#define DROMOZOA_SCOPED_PTR_HPP

#include <utility>

namespace dromozoa {
  template <class T>
  class scoped_ptr {
  public:
    explicit scoped_ptr(T* ptr = 0) : ptr_(ptr) {}

    ~scoped_ptr() {
      typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
      (void) sizeof(type_must_be_complete);
      delete ptr_;
    }

    void reset(T* ptr = 0) {
      scoped_ptr(ptr).swap(*this);
    }

    T* release() {
      T* ptr = ptr_;
      ptr_ = 0;
      return ptr;
    }

    bool valid() const {
      return ptr_;
    }

    T* get() const {
      return ptr_;
    }

    T& operator*() const {
      return *ptr_;
    }

    T* operator->() const {
      return ptr_;
    }

    void swap(scoped_ptr& that) {
      std::swap(ptr_, that.ptr_);
    }

  private:
    T* ptr_;
    scoped_ptr(const scoped_ptr&);
    scoped_ptr& operator=(const scoped_ptr&);
  };
}

#endif
