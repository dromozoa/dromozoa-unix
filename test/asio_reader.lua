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
local asio = unix.asio

unix.set_log_level(3)
unix.set_raise_error(true)

local fd = unix.fd.stdin
local selector = unix.selector()
selector:open(1024, unix.O_CLOEXEC)

local asio = asio(selector)
local fd = unix.fd.stdin
fd:ndelay_on()
asio:add(fd)

coroutine.resume(coroutine.create(function ()
  while true do
    local c = asio:read(fd, 1, 10)
    if c == nil then
      print("timeout")
    else
      print(("%q"):format(c))
    end
    if c == "" then
      break
    end
  end
  asio:stop()
end))

asio:dispatch()

asio:del(fd)
selector:close()
