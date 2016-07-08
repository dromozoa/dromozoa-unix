-- Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local uint32 = require "dromozoa.commons.uint32"
local unix = require "dromozoa.unix"

local addrinfo = assert(unix.getaddrinfo(nil, "0", { ai_socktype = unix.SOCK_STREAM, ai_flags = unix.AI_PASSIVE }))
local ai = addrinfo[1]
assert(ai.ai_socktype == unix.SOCK_STREAM)

local server = assert(unix.socket(ai.ai_family, uint32.bor(ai.ai_socktype, unix.SOCK_CLOEXEC), ai.ai_protocol))
assert(server:is_coe())
assert(server:getsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR) == 0)
assert(server:setsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR, 1))
assert(server:getsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR) ~= 0)
assert(server:bind(ai.ai_addr))
assert(server:listen())

local sa = assert(server:getsockname())
local host, serv = assert(sa:getnameinfo(uint32.bor(unix.NI_NUMERICHOST, unix.NI_NUMERICSERV)))
assert(host == "0.0.0.0" or host == "::")
assert(tonumber(serv) > 0)
assert(server:close())

local fd1, fd2 = assert(unix.socketpair(unix.AF_UNIX, uint32.bor(unix.SOCK_STREAM, unix.SOCK_CLOEXEC)))
assert(fd1:is_coe())
assert(fd2:is_coe())

local sa1 = assert(fd1:getsockname())
assert(sa1:family() == unix.AF_UNIX)
assert(sa1:path() == "")

local sa2 = assert(fd1:getpeername())
assert(sa2:family() == unix.AF_UNIX)
assert(sa2:path() == "")

assert(fd1:getsockopt(unix.SOL_SOCKET, unix.SO_ERROR) == 0)
assert(fd1:getsockopt(unix.SOL_SOCKET, unix.SO_RCVBUF) > 0);
assert(fd1:getsockopt(unix.SOL_SOCKET, unix.SO_SNDBUF) > 0);

assert(fd1:write("foo"))
assert(fd1:close())

assert(fd2:read(4) == "foo")
assert(fd2:read(4) == "")
assert(fd2:close())

assert(unix.SO_BROADCAST)
