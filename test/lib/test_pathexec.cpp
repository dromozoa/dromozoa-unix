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

#include <errno.h>
#include <stdlib.h>

#include <vector>

#include <dromozoa/pathexec.hpp>

#include "assert.h"

#define assert_pathexec_buffer_size(path, command, size) \
  do { \
    const char* argv[] = { (command), 0 }; \
    size_t result = dromozoa::pathexec_buffer_size((path), argv); \
    assert("assert_pathexec_buffer_size" && (result == (size))); \
  } while (false)

int main(int, char*[]) {
  assert_pathexec_buffer_size(0, "/foo", 0);
  assert_pathexec_buffer_size(0, "./foo", 0);
  assert_pathexec_buffer_size(0, "foo/bar", 0);
  assert_pathexec_buffer_size("", "foo", 6);
  assert_pathexec_buffer_size(".", "foo", 6);
  assert_pathexec_buffer_size(":", "foo", 6);
  assert_pathexec_buffer_size("/usr/bin:/bin:/usr/sbin:/sbin", "foo", 14);

  const char* path = getenv("PATH");
  const char* argv[] = { "no such command", 0 };
  std::vector<char> buffer(dromozoa::pathexec_buffer_size(path, argv));
  assert(dromozoa::pathexec(path, argv, 0, &buffer[0], buffer.size()) == -1);
  assert(errno == ENOENT);
  return 0;
}
