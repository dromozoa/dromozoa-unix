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

#include <stddef.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iomanip>
#include <iostream>

#include <dromozoa/compat_socket.hpp>

#include "assert.hpp"

void test_compat_socket1() {
  int fd = dromozoa::compat_socket(AF_UNIX, SOCK_STREAM | dromozoa::COMPAT_SOCK_CLOEXEC | dromozoa::COMPAT_SOCK_NONBLOCK, 0);
  std::cout << fd << "\n";
  assert(fd != -1);
  assert_coe(fd);
  assert_ndelay_on(fd);
  assert(close(fd) != -1);
}

void test_compat_socket2() {
  int fd = dromozoa::compat_socket(AF_UNIX, SOCK_STREAM | dromozoa::COMPAT_SOCK_CLOEXEC, 0);
  std::cout << fd << "\n";
  assert(fd != -1);
  assert_coe(fd);
  assert_ndelay_off(fd);
  assert(close(fd) != -1);
}

void test_compat_socketpair1() {
  int fd[2] = { -1, -1 };
  assert(dromozoa::compat_socketpair(AF_UNIX, SOCK_STREAM | dromozoa::COMPAT_SOCK_CLOEXEC | dromozoa::COMPAT_SOCK_NONBLOCK, 0, fd) != -1);
  std::cout << fd[0] << ", " << fd[1] << "\n";
  assert(fd[0] != -1);
  assert(fd[1] != -1);
  assert_coe(fd[0]);
  assert_coe(fd[1]);
  assert_ndelay_on(fd[0]);
  assert_ndelay_on(fd[1]);
  assert(close(fd[0]) != -1);
  assert(close(fd[1]) != -1);
}

void test_compat_socketpair2() {
  int fd[2] = { -1, -1 };
  assert(dromozoa::compat_socketpair(AF_UNIX, SOCK_STREAM | dromozoa::COMPAT_SOCK_CLOEXEC, 0, fd) != -1);
  std::cout << fd[0] << ", " << fd[1] << "\n";
  assert(fd[0] != -1);
  assert(fd[1] != -1);
  assert_coe(fd[0]);
  assert_coe(fd[1]);
  assert_ndelay_off(fd[0]);
  assert_ndelay_off(fd[1]);
  assert(close(fd[0]) != -1);
  assert(close(fd[1]) != -1);
}

void dump() {
  static const size_t width = sizeof(int) * 4;
  std::cout << std::hex << std::setfill('0')
      << "O_CLOEXEC=0x" << std::setw(width) << O_CLOEXEC << "\n"
      << "O_NONBLOCK=0x" << std::setw(width) << O_NONBLOCK << "\n"
      << "SOCK_STREAM=0x" << std::setw(width) << SOCK_STREAM << "\n"
      << "SOCK_DGRAM=0x" << std::setw(width) << SOCK_DGRAM << "\n"
      << "SOCK_SEQPACKET=0x" << std::setw(width) << SOCK_SEQPACKET << "\n"
#ifdef SOCK_RAW
      << "SOCK_RAW=0x" << std::setw(width) << SOCK_RAW << "\n"
#else
      << "SOCK_RAW not defined\n"
#endif
#ifdef SOCK_RDM
      << "SOCK_RDM=0x" << std::setw(width) << SOCK_RDM << "\n"
#else
      << "SOCK_RDM not defined\n"
#endif
#ifdef SOCK_PACKET
      << "SOCK_PACKET=0x" << std::setw(width) << SOCK_PACKET << "\n"
#else
      << "SOCK_PACKET not defined\n"
#endif
#ifdef SOCK_CLOEXEC
      << "SOCK_CLOEXEC=0x" << std::setw(width) << SOCK_CLOEXEC << "\n"
#else
      << "SOCK_CLOEXEC not defined\n"
#endif
#ifdef SOCK_NONBLOCK
      << "SOCK_NONBLOCK=0x" << std::setw(width) << SOCK_NONBLOCK << "\n"
#else
      << "SOCK_NONBLOCK not defined\n"
#endif
      << "COMPAT_SOCK_CLOEXEC=0x" << std::setw(width) << dromozoa::COMPAT_SOCK_CLOEXEC << "\n"
      << "COMPAT_SOCK_NONBLOCK=0x" << std::setw(width) << dromozoa::COMPAT_SOCK_NONBLOCK << "\n"
      ;
}

int main(int, char*[]) {
  test_compat_socket1();
  test_compat_socket2();
  test_compat_socketpair1();
  test_compat_socketpair2();
  dump();
  return 0;
}
