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

#include <dromozoa/argument_vector.hpp>

namespace dromozoa {
  argument_vector::argument_vector() : initialized_() {}

  void argument_vector::clear() {
    initialized_ = true;
    str_.clear();
    ptr_.clear();
  }

  void argument_vector::push_back(const char* value) {
    initialized_ = true;
    str_.push_back(value);
    ptr_.clear();
  }

  void argument_vector::push_back(const std::string& value) {
    initialized_ = true;
    str_.push_back(value);
    ptr_.clear();
  }

  const char* const* argument_vector::get() const {
    if (initialized_) {
      if (ptr_.empty()) {
        ptr_.reserve(str_.size() + 1);
        std::vector<std::string>::const_iterator i = str_.begin();
        std::vector<std::string>::const_iterator end = str_.end();
        for (; i != end; ++i) {
          ptr_.push_back((*i).c_str());
        }
        ptr_.push_back(0);
      }
      return &ptr_[0];
    } else {
      return 0;
    }
  }
}
