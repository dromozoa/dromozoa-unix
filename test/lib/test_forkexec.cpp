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
#include <stdlib.h>
#include <sys/wait.h>

#include <dromozoa/forkexec.hpp>

void test_forkexec() {
  const char* path = getenv("PATH");
  const char* argv[] = { "env", 0 };
  pid_t pid = -1;
  assert(dromozoa::forkexec(path, argv, 0, 0, 0, pid) == 0);
  assert(pid != -1);
  int status;
  assert(waitpid(-1, &status, 0) == pid);
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

int main(int, char*[]) {
  test_forkexec();
  return 0;
}
