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

local selector = unix.selector()
selector:open(1024, unix.O_CLOEXEC)

local asio = unix.asio(selector)

os.remove("test.sock")
local server = unix.socket(unix.AF_UNIX, unix.SOCK_STREAM)
server:bind(unix.sockaddr_un("test.sock"))
server:listen()
server:ndelay_on()
asio:add(server)

local count = 0

coroutine.resume(coroutine.create(function ()
  while true do
    local fd, sockaddr = asio:accept(server, unix.bor(unix.O_CLOEXEC, unix.O_NONBLOCK), 10)
    if fd == nil then
      break
    end
    assert(sockaddr:family() == unix.AF_UNIX)
    print("sockaddr", sockaddr:path())
    print("sockname", fd:getsockname():path())
    print("peername", fd:getpeername():path())
    coroutine.resume(coroutine.create(function ()
      asio:add(fd)
      while true do
        local line = asio:read_line(fd)
        if line == "" then
          break
        end
        asio:write(fd, line)
        asio:write(fd, "\n")
      end
      asio:del(fd)
      fd:close()
    end))
  end
  while count > 0 do
    asio:wait(1)
  end
  asio:stop()
end))

asio:dispatch()

asio:del(server)
server:close()
selector:close()
