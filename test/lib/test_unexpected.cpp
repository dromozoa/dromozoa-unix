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

#include <string>

#include <dromozoa/unexpected.hpp>

#include "assert.hpp"

void handler(const char* what, const char* file, int line, const char* function) {
  assert(std::string(what) == "test2");
  assert(std::string(file).find("test_unexpected.cpp") != std::string::npos);
  assert(line == 34);
  assert(std::string(function).find("main") != std::string::npos);
}

int main(int, char*[]) {
  DROMOZOA_UNEXPECTED("test1");
  dromozoa::set_unexpected(handler);
  DROMOZOA_UNEXPECTED(std::string("test2"));
  return 0;
}
