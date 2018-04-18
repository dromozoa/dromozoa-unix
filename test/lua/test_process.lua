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
local PATH = os.getenv "PATH"
local envp = unix.get_environ()
envp[#envp + 1] = "foo=bar"

local reader, writer = assert(unix.pipe())
local process = assert(unix.process())
assert(process:forkexec(PATH, { "sh", "-c", [[pwd; echo "$foo"]] }, envp, "/", { [1] = writer }))
assert(writer:close())

local buffer = {}
while true do
  local data = assert(reader:read(256))
  if data == "" then
    break
  else
    buffer[#buffer + 1] = data
  end
end
assert(reader:close())
if verbose then
  print(("%q"):format(table.concat(buffer)))
end
assert(table.concat(buffer) == "/\nbar\n")

local pid, reason, status = assert(unix.wait())
assert(pid == process[1])
assert(reason == "exit")
assert(status == 0)

local process = unix.process()
local result, message, code = process:forkexec(PATH, { "no such command" })
if verbose then
  print(message)
end
assert(not result)
assert(code == unix.ENOENT)

local pid, reason, status = assert(unix.wait())
assert(pid == process[1])
assert(reason == "exit")
assert(status == 1)

local reader, writer = assert(unix.pipe(0))
assert(reader:coe())
local process = assert(unix.process())
assert(process:forkexec_daemon(PATH, { "sh", "-c", "echo foo >&" .. writer:get() .. "; exec sleep 10" }))
assert(writer:close())

local pid, reason, status = assert(unix.wait())
assert(pid == process[1])
assert(reason == "exit")
assert(status == 0)

local timer = unix.timer()
timer:start()
assert(reader:read(4) == "foo\n")
assert(unix.kill(process[2], 0))
assert(unix.kill(process[2]))
assert(reader:read(4) == "")
timer:stop()
if verbose then
  print(timer:elapsed())
end
assert(timer:elapsed() < 5)

local process = unix.process()
local result, message, code = process:forkexec_daemon(PATH, { "no such command" })
if verbose then
  print(message)
end
assert(not result)
assert(code == unix.ENOENT)

local pid, reason, status = assert(unix.wait())
assert(pid == process[1])
assert(reason == "exit")
assert(status == 0)
