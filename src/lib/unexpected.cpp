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

#include <iostream>

#include <dromozoa/unexpected.hpp>

namespace dromozoa {
  namespace {
    unexpected_handler handler_ = unexpected_cerr;
  }

  void unexpected_noop(const char*, const char*, int, const char*) {}

  void unexpected_cerr(const char* what, const char* file, int line, const char* function) {
    std::cerr << "unexpected";
    if (what) {
      std::cerr << ": " << what;
    }
    if (file) {
      std::cerr << " at " << file << ":" << line;
    }
    if (function) {
      std::cerr << " in " << function;
    }
    std::cerr << std::endl;
  }

  void set_unexpected(unexpected_handler handler) {
    handler_ = handler;
  }

  void unexpected(const char* what, const char* file, int line, const char* function) {
    try {
      if (handler_) {
        handler_(what, file, line, function);
      }
    } catch (...) {}
  }

  void unexpected(const std::string& what, const char* file, int line, const char* function) {
    unexpected(what.c_str(), file, line, function);
  }
}
