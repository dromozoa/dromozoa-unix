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

local unix = require "dromozoa.unix"

local addrinfo = assert(unix.getaddrinfo(nil, "0", { ai_socktype = unix.SOCK_STREAM, ai_flags = unix.AI_PASSIVE }))
local ai = addrinfo[1]
assert(ai.ai_socktype == unix.SOCK_STREAM)
local server = assert(unix.socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol))
assert(server:getsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR) == 0)
assert(server:setsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR, 1))
-- print(server:getsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR))
assert(server:getsockopt(unix.SOL_SOCKET, unix.SO_REUSEADDR) ~= 0)
assert(server:bind(ai.ai_addr))
assert(server:listen())
local sa = assert(server:getsockname())
local host, serv = sa:getnameinfo(unix.NI_NUMERICHOST + unix.NI_NUMERICSERV)
-- print(host, serv)
assert(host == "0.0.0.0" or host == "::")
assert(tonumber(serv) > 0)
server:close()
