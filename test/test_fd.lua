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

assert(unix.get_log_level() == 0)
unix.set_log_level(3)
assert(unix.get_log_level() == 3)
unix.set_log_level(0)

local fd

do
  local reader, writer = unix.pipe()
  fd = { reader:coe():ndelay_on():get(), writer:get() }
  assert(reader:close())
  assert(reader:close())
end
collectgarbage()
collectgarbage()

assert(unix.fd.get(0) == 0)
assert(not unix.fd.close(-2))

do
  local reader, writer = unix.pipe(unix.O_CLOEXEC)
  assert(reader:get() == fd[1])
  assert(writer:get() == fd[2])
  assert(reader:close():get() == -1)
  assert(writer:close():get() == -1)
end

do
  local stdout = unix.fd(1, true)
  stdout:write("foo\n")
end
collectgarbage()
collectgarbage()
unix.fd.write(1, "bar\n")

assert(unix.STDIN_FILENO == 0)
assert(unix.STDOUT_FILENO == 1)
assert(unix.STDERR_FILENO == 2)

unix.fd.stderr:write("baz\n")
-- print(json.encode(unix.fd))

do
  local reader, writer = unix.pipe(unix.bor(unix.O_CLOEXEC, unix.O_NONBLOCK))
  writer:write("foobarbaz")
  assert(reader:read(3) == "foo")
  assert(reader:read(3) == "bar")
  assert(reader:read(3) == "baz")
  local result = reader:read(3)
  assert(result == unix.resource_unavailable_try_again)
  writer:write("qux")
  writer:close()
  assert(reader:read(4) == "qux")
  assert(reader:read(4) == "")
  reader:close()
end
