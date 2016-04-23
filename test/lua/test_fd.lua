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

local uint32 = require "dromozoa.commons.uint32"
local unix = require "dromozoa.unix"

assert(unix.fd.get(0) == 0)
assert(unix.stdin:get() == 0)
assert(not unix.fd.close(-2))

local fd

do
  local reader, writer = unix.pipe()
  assert(reader:is_coe())
  assert(reader:is_ndelay_off())
  assert(writer:is_coe())
  assert(writer:is_ndelay_off())
  fd = { reader:ndelay_on():get(), writer:get() }
  assert(reader:close())
  assert(not reader:close())
end
collectgarbage()
collectgarbage()

do
  local reader, writer = unix.pipe()
  assert(reader:get() == fd[1])
  assert(writer:get() == fd[2])
  assert(reader:close():get() == -1)
  assert(writer:close():get() == -1)
end

do
  local stdout = assert(unix.fd_ref(1))
  assert(stdout:write("foo\n"))
end
collectgarbage()
collectgarbage()
assert(unix.fd.write(1, "bar\n"))
assert(unix.stdout:write("baz\n"))
assert(unix.stderr:write("qux\n"))

assert(unix.STDIN_FILENO == 0)
assert(unix.STDOUT_FILENO == 1)
assert(unix.STDERR_FILENO == 2)

local reader, writer = assert(unix.pipe(uint32.bor(unix.O_CLOEXEC, unix.O_NONBLOCK)))
assert(writer:write("foobarbaz"))
assert(assert(reader:read(3)) == "foo")
assert(assert(reader:read(3)) == "bar")
assert(assert(reader:read(3)) == "baz")
local a, b, c = reader:read(3)
assert(a == nil)
assert(c == unix.EAGAIN or c == unix.EWOULDBLOCK)
assert(writer:write("qux"))
assert(writer:close())
assert(reader:read(4) == "qux")
assert(reader:read(4) == "")
assert(reader:close())
