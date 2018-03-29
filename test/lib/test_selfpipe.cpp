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
#include <stdlib.h>
#include <sys/wait.h>

#include <iostream>

#include <dromozoa/selfpipe.hpp>
#include <dromozoa/forkexec.hpp>
#include <dromozoa/selector.hpp>

#include "check.hpp"

int main(int, char*[]) {
  assert(dromozoa::selfpipe_open() == 0);

  dromozoa::selector_impl* impl = dromozoa::selector::open(dromozoa::SELECTOR_CLOEXEC);
  assert(impl);
  int fd = impl->get();
  std::cout << fd << "\n";
  assert(fd != -1);
  check_coe(fd);
  check_ndelay_off(fd);

  dromozoa::selector selector(impl);
  assert(selector.add(dromozoa::selfpipe_get(), dromozoa::SELECTOR_READ) == 0);

  const char* path = getenv("PATH");
  const char* argv[] = { "ls", "-l", "/", 0 };
  pid_t pid = -1;
  assert(dromozoa::forkexec(path, argv, 0, 0, 0, pid) == 0);
  std::cout << pid << "\n";
  assert(pid != -1);

  while (true) {
    int result = selector.select(0);
    std::cout << result << "\n";
    if (result == -1) {
      assert(errno == EINTR);
    } else {
      assert(result == 1);
      break;
    }
  }
  assert(dromozoa::selfpipe_read() == 1);

  int status = 0;
  assert(waitpid(-1, &status, 0) == pid);
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);

  assert(dromozoa::selfpipe_close() == 0);
  return 0;
}
