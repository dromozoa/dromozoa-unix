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
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <iostream>
#include <string>

#include <dromozoa/compat_pipe2.hpp>
#include <dromozoa/compat_sigmask.hpp>
#include <dromozoa/file_descriptor.hpp>
#include <dromozoa/forkexec.hpp>

void check_sigmask() {
  sigset_t mask;
  assert(dromozoa::compat_sigmask(SIG_BLOCK, 0, &mask) == 0);
  assert(sigismember(&mask, SIGCHLD));
  assert(!sigismember(&mask, SIGTERM));
}

void test_forkexec1() {
  std::cout << "======== test_forkexec1\n";
  const char* path = getenv("PATH");
  const char* argv[] = { "echo", "foo", 0 };
  pid_t pid = -1;
  assert(dromozoa::forkexec(path, argv, 0, 0, 0, pid) == 0);
  assert(pid != -1);
  std::cout << "child pid " << pid << "\n";
  int status;
  assert(waitpid(-1, &status, 0) == pid);
  std::cout << "status " << status << "\n";
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

void test_forkexec2() {
  std::cout << "======== test_forkexec2\n";
  const char* path = getenv("PATH");
  const char* argv[] = { "echo", "foo", 0 };
  int fd[2] = { -1, -1 };
  assert(dromozoa::compat_pipe2(fd, O_CLOEXEC) != -1);
  dromozoa::file_descriptor reader(fd[0]);
  dromozoa::file_descriptor writer(fd[1]);
  int dup2_stdio[] = { -1, writer.get(), -1 };
  pid_t pid = -1;
  assert(dromozoa::forkexec(path, argv, 0, 0, dup2_stdio, pid) == 0);
  assert(pid != -1);
  std::cout << "child pid " << pid << "\n";
  writer.close();
  char buffer[5] = { 0 };
  assert(read(reader.get(), buffer, 5) == 4);
  assert(buffer[4] == '\0');
  assert(std::string(buffer) == "foo\n");
  reader.close();
  int status;
  assert(waitpid(-1, &status, 0) == pid);
  std::cout << "status " << status << "\n";
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

void test_forkexec3() {
  std::cout << "======== test_forkexec3\n";
  const char* path = getenv("PATH");
  const char* argv[] = { "no such command", 0 };
  pid_t pid = -1;
  assert(dromozoa::forkexec(path, argv, 0, 0, 0, pid) == -1);
  assert(pid != -1);
  assert(errno == ENOENT);
  std::cout << "child pid " << pid << "\n";
  int status;
  assert(waitpid(-1, &status, 0) == pid);
  std::cout << "status " << status << "\n";
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
}

void test_forkexec_daemon1() {
  std::cout << "======== test_forkexec_daemon1\n";
  const char* path = getenv("PATH");
  const char* argv[] = { "sleep", "42", 0 };
  pid_t pid1 = -1;
  pid_t pid2 = -1;
  assert(dromozoa::forkexec_daemon(path, argv, 0, "/", pid1, pid2) == 0);
  assert(pid1 != -1);
  assert(pid2 != -1);
  std::cout << "child pid " << pid1 << " " << pid2 << "\n";
  int status;
  assert(waitpid(-1, &status, 0) == pid1);
  std::cout << "status " << status << "\n";
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
  assert(kill(pid2, SIGTERM) == 0);
}

void test_forkexec_daemon2() {
  std::cout << "======== test_forkexec_daemon2\n";
  const char* path = getenv("PATH");
  const char* argv[] = { "no such command", 0 };
  pid_t pid1 = -1;
  pid_t pid2 = -1;
  assert(dromozoa::forkexec_daemon(path, argv, 0, "/", pid1, pid2) == -1);
  assert(pid1 != -1);
  assert(pid2 != -1);
  assert(errno = ENOENT);
  std::cout << "child pid " << pid1 << " " << pid2 << "\n";
  int status;
  assert(waitpid(-1, &status, 0) == pid1);
  std::cout << "status " << status << "\n";
  assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

int main(int, char*[]) {
  sigset_t mask;
  assert(sigemptyset(&mask) == 0);
  assert(sigaddset(&mask, SIGCHLD) == 0);
  assert(dromozoa::compat_sigmask(SIG_BLOCK, &mask, 0) == 0);
  check_sigmask();
  std::cout << "pid " << getpid() << "\n";
  test_forkexec1();
  check_sigmask();
  test_forkexec2();
  check_sigmask();
  test_forkexec3();
  check_sigmask();
  test_forkexec_daemon1();
  check_sigmask();
  test_forkexec_daemon2();
  check_sigmask();
  return 0;
}
