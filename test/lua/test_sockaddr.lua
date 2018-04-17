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

local result, message = pcall(unix.sockaddr_un, ("0123456789abcdef"):rep(4096))
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

local sa = assert(unix.sockaddr_un "test.sock")
if verbose then
  io.stderr:write(sa:size(), "\n")
end
assert(sa:family() == unix.AF_UNIX)
assert(sa:path() == "test.sock")

local sa = assert(unix.sockaddr_un "\0test.sock")
assert(sa:family() == unix.AF_UNIX)
assert(sa:path() == "\0test.sock")

local result, message = pcall(unix.sockaddr_un, ("X"):rep(sa:size() * 2))
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

local addrinfo = assert(unix.getaddrinfo("127.0.0.1", "http", { ai_family = unix.AF_INET, ai_socktype = unix.SOCK_STREAM }))
local ai = addrinfo[1]
assert(ai.ai_addr:addr() == "\127\0\0\1")
assert(ai.ai_addr:port() == "\0\80")

local addrinfo = assert(unix.getaddrinfo("::1", "http", { ai_family = unix.AF_INET6, ai_socktype = unix.SOCK_STREAM }))
local ai = addrinfo[1]
assert(ai.ai_addr:addr() == ("\0"):rep(15) .. "\1")
assert(ai.ai_addr:port() == "\0\80")

local sa = assert(unix.sockaddr_in("\127\0\0\1", "\0\80"))
assert(sa:family() == unix.AF_INET)
local host, serv = assert(sa:getnameinfo(unix.NI_NUMERICHOST + unix.NI_NUMERICSERV))
assert(host == "127.0.0.1")
assert(serv == "80")

local sa = assert(unix.sockaddr_in6(("\0"):rep(15) .. "\1", "\0\80"))
assert(sa:family() == unix.AF_INET6)
local host, serv = assert(sa:getnameinfo(unix.NI_NUMERICHOST + unix.NI_NUMERICSERV))
assert(host == "::1")
assert(serv == "80")
