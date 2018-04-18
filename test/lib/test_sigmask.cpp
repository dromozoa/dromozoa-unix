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
#include <signal.h>

#include <dromozoa/compat_sigmask.hpp>
#include <dromozoa/sigmask.hpp>

void test_sigmask1() {
  sigset_t mask1;
  sigset_t mask2;
  sigemptyset(&mask1);
  sigaddset(&mask1, SIGCHLD);

  assert(dromozoa::compat_sigmask(SIG_BLOCK, &mask1, &mask2) != -1);
  assert(sigismember(&mask1, SIGCHLD));
  assert(!sigismember(&mask2, SIGCHLD));

  {
    dromozoa::sigmask_saver save_sigmask(mask2);
    assert(dromozoa::compat_sigmask(SIG_BLOCK, 0, &mask1) != -1);
  }
  assert(dromozoa::compat_sigmask(SIG_BLOCK, 0, &mask2) != -1);

  assert(sigismember(&mask1, SIGCHLD));
  assert(!sigismember(&mask2, SIGCHLD));
}

void test_sigmask2() {
  sigset_t mask;
  assert(dromozoa::sigmask_block_all_signals(&mask) != -1);
  assert(!sigismember(&mask, SIGCHLD));
  assert(dromozoa::sigmask_unblock_all_signals(&mask) != -1);
  assert(sigismember(&mask, SIGCHLD));
}

int main(int, char*[]) {
  test_sigmask1();
  test_sigmask2();
  return 0;
}
