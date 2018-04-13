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
#include <unistd.h>
#include <sys/wait.h>

#include <iostream>

#include <dromozoa/selfpipe.hpp>
#include <dromozoa/forkexec.hpp>
#include <dromozoa/selector.hpp>

int main(int, char*[]) {
  dromozoa::selfpipe_impl* impl = dromozoa::selfpipe::open();
  assert(impl);
  dromozoa::selfpipe sp1(impl);

  dromozoa::selfpipe sp2(dromozoa::selfpipe::open());

  std::cout << "pid " << getpid() << "\n";

  dromozoa::selector selector(dromozoa::selector::open(dromozoa::SELECTOR_CLOEXEC));
  assert(selector.add(sp1.get(), dromozoa::SELECTOR_READ) == 0);

  const char* path = getenv("PATH");
  const char* argv[] = { "echo", "foo", 0 };
  pid_t pid = -1;
  assert(dromozoa::forkexec(path, argv, 0, 0, 0, pid) == 0);
  std::cout << "child pid " << pid << "\n";
  assert(pid != -1);

  while (true) {
    int result = selector.select(0);
    std::cout << "result " << result << "\n";
    if (result == -1) {
      assert(errno == EINTR);
    } else {
      assert(result == 1);
      break;
    }
  }
  assert(sp1.read() == 1);

  int status = 0;
  assert(waitpid(pid, &status, 0) == pid);
  std::cout << "status " << status << "\n";
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);

  assert(sp1.close() == 0);

  assert(sp2.read() == 1);
  assert(sp2.read() == 0);

  return 0;
}
