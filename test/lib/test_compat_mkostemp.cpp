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

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <dromozoa/compat_mkostemp.hpp>

int main(int, char*[]) {
  char buffer[] = { '/', 't', 'm', 'p', '/', 'd', 'r', 'o', 'm', 'o', 'z', 'o', 'a', '-', 'X', 'X', 'X', 'X', 'X', 'X', '\0' };
  int fd = dromozoa::compat_mkostemp(buffer, O_CLOEXEC);
  assert(fd != -1);

  std::string filename = buffer;
  std::cout << filename << "\n";
  assert(filename != "/tmp/dromozoa-XXXXXX");
  assert(filename.find("/tmp/dromozoa-") == 0);

  assert(write(fd, "foo\n", 4) == 4);
  assert(write(fd, "bar\n", 4) == 4);
  assert(write(fd, "baz\n", 4) == 4);
  assert(write(fd, "qux\n", 4) == 4);
  assert(close(fd) == 0);

  std::ifstream in(filename.c_str());
  assert(in);
  unlink(filename.c_str());

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(in, line)) {
    std::cout << line << "\n";
    lines.push_back(line);
  }
  assert(lines[0] == "foo");
  assert(lines[1] == "bar");
  assert(lines[2] == "baz");
  assert(lines[3] == "qux");

  return 0;
}
