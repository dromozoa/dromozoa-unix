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

#ifndef DROMOZOA_COMPAT_SOCKET_HPP
#define DROMOZOA_COMPAT_SOCKET_HPP

namespace dromozoa {
  extern const int COMPAT_SOCK_CLOEXEC;
  extern const int COMPAT_SOCK_NONBLOCK;
  int compat_socket(int domain, int type, int protocol);
  int compat_socketpair(int domain, int type, int protocol, int socket_fd[2]);
}

#endif
