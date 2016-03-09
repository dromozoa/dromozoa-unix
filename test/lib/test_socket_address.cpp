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
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

#include <dromozoa/socket_address.hpp>

int main(int, char*[]) {
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(80);
  in_addr_t in_addr = inet_addr("127.0.0.1");
  memmove(&sin.sin_addr, &in_addr, sizeof(sin.sin_addr));
  dromozoa::socket_address socket_address(reinterpret_cast<const struct sockaddr*>(&sin), sizeof(sin));

  char nodename[NI_MAXHOST];
  char servname[NI_MAXSERV];
  assert(getnameinfo(socket_address.get(), socket_address.size(), nodename, NI_MAXHOST, servname, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0);

  assert(std::string(nodename) == "127.0.0.1");
  assert(std::string(servname) == "80");

  return 0;
}
