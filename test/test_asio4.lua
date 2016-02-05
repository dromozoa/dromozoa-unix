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

unix.set_log_level(2)
unix.set_raise_error(true)

local reader, writer = unix.socketpair(unix.AF_UNIX, unix.SOCK_STREAM)
reader:ndelay_on()
writer:ndelay_on()

local selector = unix.selector():open(16, unix.O_CLOEXEC)
local asio = unix.asio(selector)

asio:add(reader)
asio:add(writer)

asio:resume(function ()
  asio:write(writer, "0123456789")
  assert(asio:read_some(reader, 4, 1) == "0123")
  assert(asio:read_some(reader, 4, 1) == "4567")
  assert(asio:read_some(reader, 4, 1) == "89")
  asio:write(writer, "0123456789")
  assert(asio:read(reader, 4, 1) == "0123")
  assert(asio:read_some(reader, 4, 1) == "4567")
  assert(asio:read_some(reader, 4, 1) == "89")
  asio:stop()
end)
asio:dispatch()

asio:del(reader)
asio:del(writer)
reader:close()
writer:close()
selector:close()
