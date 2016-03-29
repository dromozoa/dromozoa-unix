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

#ifndef DROMOZOA_UNEXPECTED_HPP
#define DROMOZOA_UNEXPECTED_HPP

#include <string>

namespace dromozoa {
  typedef void (*unexpected_handler)(const char* what, const char* file, int line, const char* function);
  void unexpected_noop(const char* what, const char* file, int line, const char* function);
  void unexpected_cerr(const char* what, const char* file, int line, const char* function);
  void set_unexpected(unexpected_handler handler);
  void unexpected(const char* what, const char* file, int line, const char* function);
  void unexpected(const std::string& what, const char* file, int line, const char* function);
}

#define DROMOZOA_UNEXPECTED(what) \
  dromozoa::unexpected(what, __FILE__, __LINE__, __func__)

#endif
