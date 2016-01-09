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

local addrinfo = assert(unix.getaddrinfo("127.0.0.1", "http", { ai_family = unix.AF_INET, ai_socktype = unix.SOCK_STREAM }))
assert(#addrinfo == 1)

local ai = addrinfo[1]
local host, serv = assert(ai.ai_addr:getnameinfo(unix.NI_NUMERICHOST + unix.NI_NUMERICSERV))
assert(ai.ai_socktype == unix.SOCK_STREAM)
assert(ai.ai_addr:family() == unix.AF_INET)
assert(host == "127.0.0.1")
assert(tonumber(serv) == 80)
-- print(ai.ai_addr:family(), host, serv)
