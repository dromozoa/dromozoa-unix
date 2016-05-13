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

local sequence = require "dromozoa.commons.sequence"
local sequence_writer = require "dromozoa.commons.sequence_writer"
local unix = require "dromozoa.unix"

local PATH = os.getenv("PATH")
local envp = unix.get_environ()
sequence.push(envp, "foo=bar")

local reader, writer = assert(unix.pipe())
local process = assert(unix.process())
assert(process:forkexec(PATH, { "sh", "-c", "pwd; echo \"$foo\"" }, envp, "/", { [1] = writer }))
assert(writer:close())

local out = sequence_writer()
while true do
  local data = assert(reader:read(4096))
  if data == "" then
    break
  else
    out:write(data)
  end
end
assert(reader:close())
assert(out:concat() == "/\nbar\n")

local a, b, c = assert(unix.wait())
assert(a == process[1])
assert(b == "exit")
assert(c == 0)

local process = unix.process()
local a, b, c = process:forkexec(PATH, { "no such command" })
assert(a == nil)
assert(c == unix.ENOENT)

local a, b, c = assert(unix.wait())
assert(a == process[1])
assert(b == "exit")
assert(c == 1)

local reader, writer = assert(unix.pipe(0))
assert(reader:coe())
local process = assert(unix.process())
assert(process:forkexec_daemon(PATH, { "sh", "-c", "echo foo >&" .. writer:get() .. "; exec sleep 10" }))
assert(writer:close())

local a, b, c = assert(unix.wait())
assert(a == process[1])
assert(b == "exit")
assert(c == 0)

local t1 = unix.clock_gettime(unix.CLOCK_MONOTONIC_RAW)
assert(reader:read(4) == "foo\n")
assert(unix.kill(process[2], 0))
assert(unix.kill(process[2]))
assert(reader:read(4) == "")
local t2 = unix.clock_gettime(unix.CLOCK_MONOTONIC_RAW)
assert(t2 - t1 < unix.timespec(5))

local process = unix.process()
local a, b, c = process:forkexec_daemon(PATH, { "no such command" })
assert(a == nil)
assert(c == unix.ENOENT)

local a, b, c = assert(unix.wait())
assert(a == process[1])
assert(b == "exit")
assert(c == 0)
