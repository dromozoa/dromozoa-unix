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

#ifndef DROMOZOA_SOCKET_ADDRESS_HPP
#define DROMOZOA_SOCKET_ADDRESS_HPP

#include <sys/socket.h>

namespace dromozoa {
  class socket_address {
  public:
    socket_address();
    socket_address(const struct sockaddr* address, socklen_t size);
    struct sockaddr* get();
    socklen_t* size_ptr();
    const struct sockaddr* get() const;
    socklen_t size() const;
    sa_family_t family() const;
  private:
    struct sockaddr_storage address_;
    socklen_t size_;
  };
}

#endif
