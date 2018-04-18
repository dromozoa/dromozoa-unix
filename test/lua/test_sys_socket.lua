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

local addrinfo = assert(unix.getaddrinfo(nil, "0", { ai_socktype = unix.SOCK_STREAM, ai_flags = unix.AI_PASSIVE }))
local ai = addrinfo[1]
assert(ai.ai_socktype == unix.SOCK_STREAM)

local server = assert(unix.socket(ai.ai_family, unix.bor(ai.ai_socktype, unix.SOCK_CLOEXEC), ai.ai_protocol))
assert(server:is_coe())
assert(server:getsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR) == 0)
assert(server:setsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR, 1))
assert(server:getsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR) ~= 0)
assert(server:bind(ai.ai_addr))
assert(server:listen())

local sa = assert(server:getsockname())
local host, serv = assert(sa:getnameinfo(unix.bor(unix.NI_NUMERICHOST, unix.NI_NUMERICSERV)))
if verbose then
  io.stderr:write(host, "\n")
  io.stderr:write(serv, "\n")
end
assert(host == "0.0.0.0" or host == "::")
assert(tonumber(serv) > 0)
assert(server:close())

local fd1, fd2 = assert(unix.socketpair(unix.AF_UNIX, unix.bor(unix.SOCK_STREAM, unix.SOCK_CLOEXEC)))
assert(fd1:is_coe())
assert(fd2:is_coe())

local sa1 = assert(fd1:getsockname())
assert(sa1:family() == unix.AF_UNIX)
assert(sa1:path() == "")

local sa2 = assert(fd1:getpeername())
assert(sa2:family() == unix.AF_UNIX)
assert(sa2:path() == "")

local so_error = fd1:getsockopt(unix.SOL_SOCKET, unix.SO_ERROR)
local so_rcvbuf = fd1:getsockopt(unix.SOL_SOCKET, unix.SO_RCVBUF)
local so_sndbuf = fd1:getsockopt(unix.SOL_SOCKET, unix.SO_SNDBUF)
if verbose then
  io.stderr:write(so_rcvbuf, "\n")
  io.stderr:write(so_sndbuf, "\n")
end
assert(so_error == 0)
assert(so_rcvbuf > 0);
assert(so_sndbuf > 0);

assert(fd1:write "foo")
assert(fd1:close())

assert(fd2:read(4) == "foo")
assert(fd2:read(4) == "")
assert(fd2:close())
