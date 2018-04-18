-- Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-unix.
--
-- dromozoa-unix is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-unix is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-unix.  If not, see <http://www.gnu.org/licenses/>.

local unix = require "dromozoa.unix"

local verbose = os.getenv "VERBOSE" == "1"

local addrinfo = assert(unix.getaddrinfo("127.0.0.1", nil, { ai_socktype = unix.SOCK_DGRAM, ai_flags = unix.AI_PASSIVE }))
local ai = addrinfo[1]
local server_fd = assert(unix.socket(ai.ai_family, unix.bor(ai.ai_socktype, unix.SOCK_CLOEXEC), ai.ai_protocol))
assert(server_fd:bind(ai.ai_addr))

local address = assert(server_fd:getsockname())
if verbose then
  local host, serv = assert(address:getnameinfo(unix.bor(unix.NI_NUMERICHOST, unix.NI_NUMERICSERV)))
  io.stderr:write(host, "\n")
  io.stderr:write(serv, "\n")
end

local client_fd = assert(unix.socket(ai.ai_family, unix.bor(ai.ai_socktype, unix.SOCK_CLOEXEC), ai.ai_protocol))
assert(client_fd:sendto("foobarbaz", nil, nil, nil, address) == 9)

local result, recv_address = assert(server_fd:recvfrom(16))
if verbose then
  local host, serv = assert(recv_address:getnameinfo(unix.bor(unix.NI_NUMERICHOST, unix.NI_NUMERICSERV)))
  io.stderr:write(host, "\n")
  io.stderr:write(serv, "\n")
end
assert(result == "foobarbaz")

assert(client_fd:sendto("foobarbaz", 4, 6, nil, address) == 3)
assert(server_fd:recv(16) == "bar")

assert(client_fd:sendto("foobarbaz", -6, -4, nil, address) == 3)
assert(server_fd:recv(16) == "bar")

assert(client_fd:sendto("foobarbaz", 6, 4, nil, address) == 0)

assert(client_fd:close())
assert(server_fd:close())
