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

unix.set_log_level(3)
unix.set_raise_error(true)

-- local reader, writer = unix.pipe(unix.bor(unix.O_CLOEXEC, unix.O_NONBLOCK))
local reader, writer = unix.socketpair(unix.AF_UNIX, unix.SOCK_STREAM)
reader:ndelay_on()
writer:ndelay_on()

local selector = unix.selector()
selector:open(1024, unix.O_CLOEXEC)

local asio = unix.asio(selector)
asio:add(reader)
asio:add(writer)

coroutine.resume(coroutine.create(function ()
  local data = ("x"):rep(65536)
  while true do
    local result, size = asio:write(writer, data, 0.2)
    print(result, size)
    if result == nil then
      break
    end
  end
  local size = 0
  while true do
    local result = asio:read(reader, 16, 0.2)
    -- print(result)
    if result == nil then
      break
    end
    size = size + #result
  end
  print(size)
  asio:stop()
end))

print("dispatch")
asio:dispatch()

asio:del(reader)
asio:del(writer)
reader:close()
writer:close()
selector:close()
