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

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include <iostream>
#include <vector>

#include <dromozoa/pathexec.hpp>

void check_pathexec_buffer_size(const char* path, const char* command, size_t expect) {
  const char* argv[] = { command, 0 };
  size_t result = dromozoa::pathexec_buffer_size(path, argv);
  assert(result == expect);
}

void test1() {
  check_pathexec_buffer_size(0, "/foo", 0);
  check_pathexec_buffer_size(0, "./foo", 0);
  check_pathexec_buffer_size(0, "foo/bar", 0);
  check_pathexec_buffer_size("", "foo", 6);
  check_pathexec_buffer_size(".", "foo", 6);
  check_pathexec_buffer_size(":", "foo", 6);
  check_pathexec_buffer_size("/usr/bin:/bin:/usr/sbin:/sbin", "foo", 14);
}

void test2() {
  const char* argv[] = { "./no such command", 0 };
  assert(dromozoa::pathexec(0, argv, 0, 0, 0) == -1);
  assert(errno == ENOENT);
}

void test3() {
  const char* argv[] = { "./.", 0 };
  assert(dromozoa::pathexec(0, argv, 0, 0, 0) == -1);
  assert(errno == EACCES);
}

void test4() {
  const char* path = getenv("PATH");
  const char* argv[] = { "no such command", 0 };
  std::vector<char> buffer(dromozoa::pathexec_buffer_size(path, argv));
  assert(dromozoa::pathexec(path, argv, 0, &buffer[0], buffer.size()) == -1);
  assert(errno == ENOENT);
}

int main(int, char*[]) {
  test1();
  test2();
  test3();
  test4();
  return 0;
}
