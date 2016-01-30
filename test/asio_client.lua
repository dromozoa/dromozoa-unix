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

local pairs = require "dromozoa.commons.pairs"
local unix = require "dromozoa.unix"

unix.set_log_level(3)
-- unix.set_raise_error(true)

local host, serv = ...

local selector = unix.selector()
selector:open(1024, unix.O_CLOEXEC)

local asio = unix.asio(selector)
print(asio.selector_timeout)

local addrinfo = unix.getaddrinfo(host, serv, { ai_family = unix.AF_INET, ai_socktype = unix.SOCK_STREAM })
local ai = addrinfo[1]

assert(ai.ai_family == unix.AF_INET)
assert(ai.ai_socktype == unix.SOCK_STREAM)
local fd = unix.socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol)
fd:ndelay_on()
asio:add(fd)

coroutine.resume(coroutine.create(function ()
  asio:wait()
  print("connect")
  local a, b, c, d = pcall(asio.connect, asio, fd, ai.ai_addr, 4)
  print("connected", a, b, c, d)
  asio:del(fd)
  fd:close()
  asio:stop()
  print("done")
end))

print("dispatch")
asio:dispatch()
print("dispatched")

selector:close()
