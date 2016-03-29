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

#include <string.h>

#include <dromozoa/socket_address.hpp>

namespace dromozoa {
  socket_address::socket_address() : address_(), size_(sizeof(address_)) {}

  socket_address::socket_address(const struct sockaddr* address, socklen_t size) : address_(), size_(size) {
    memmove(&address_, address, size);
  }

  struct sockaddr* socket_address::get() {
    return reinterpret_cast<struct sockaddr*>(&address_);
  }

  socklen_t* socket_address::size_ptr() {
    return &size_;
  }

  const struct sockaddr* socket_address::get() const {
    return reinterpret_cast<const struct sockaddr*>(&address_);
  }

  socklen_t socket_address::size() const {
    return size_;
  }

  sa_family_t socket_address::family() const {
    return get()->sa_family;
  }
}
