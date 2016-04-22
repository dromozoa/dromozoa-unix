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

#include <assert.h>

#include <string>

#include <dromozoa/argument_vector.hpp>

void test1() {
  dromozoa::argument_vector args;
  const char* const* argv = args.get();
  assert(argv == 0);
}

void test2() {
  dromozoa::argument_vector args;
  args.clear();
  const char* const* argv = args.get();
  assert(argv);
  assert(argv[0] == 0);
}

void test3() {
  dromozoa::argument_vector args;
  args.push_back("foo");
  args.push_back("bar");
  args.push_back("baz");
  const char* const* argv = args.get();
  assert(argv);
  assert(std::string(argv[0]) == "foo");
  assert(std::string(argv[1]) == "bar");
  assert(std::string(argv[2]) == "baz");
  assert(argv[3] == 0);
}

int main(int, char*[]) {
  test1();
  test2();
  test3();
  return 0;
}
