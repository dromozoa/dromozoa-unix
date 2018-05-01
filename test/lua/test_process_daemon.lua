-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

if os.getenv "SKIP_TEST_PROCESS_DAEMON" ~= "1" then
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
    io.stderr:write(timer:elapsed(), "\n")
  end
  assert(timer:elapsed() < 5)
end

if os.getenv "SKIP_TEST_PROCESS_NO_SUCH_COMMAND" ~= "1" then
  local process = unix.process()
  local result, message, code = process:forkexec_daemon(PATH, { "no such command" })
  if verbose then
    io.stderr:write(message, "\n")
  end
  assert(not result)
  assert(code == unix.ENOENT)

  local pid, reason, status = assert(unix.wait())
  assert(pid == process[1])
  assert(reason == "exit")
  assert(status == 0)
end
