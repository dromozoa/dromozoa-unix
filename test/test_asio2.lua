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

local reader, writer = unix.pipe(unix.bor(unix.O_CLOEXEC, unix.O_NONBLOCK))

local selector = unix.selector()
selector:open(1024, unix.O_CLOEXEC)

local asio = unix.asio(selector)
asio.selector_timeout = unix.timespec(0.1)

asio:add(reader)
asio:add(writer)

asio:resume(function ()
  for i = 1, 10 do
    local line, char = asio:read_line(reader)
    print("read", line)
    assert(line == tostring(i))
    assert(char == "\n")
  end
  assert(asio:read_line(reader) == "")
  asio:del(reader)
  reader:close()
  asio:stop()
end)

asio:resume(function ()
  for i = 1, 10 do
    asio:wait(0.2)
    asio:write(writer, i .. "\n")
    print("write", i)
  end
  asio:del(writer)
  writer:close()
end)

asio:dispatch()

selector:close()
