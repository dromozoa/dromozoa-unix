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

local lua
if _G["dromozoa.bind.driver"] then
  lua = "lua"
else
  lua = arg[-1]
end

assert(unix.block_signal(unix.SIGCHLD))

local selfpipe = assert(unix.selfpipe())

assert(unix.nanosleep(0.2))

local result, message, code, t = unix.nanosleep(-1)
if verbose then
  io.stderr:write(message, "\n")
  io.stderr:write(tostring(t), "\n")
end
assert(not result)
assert(code == unix.EINVAL)
assert(t.tv_sec == 0)
assert(t.tv_nsec == 0)

local process = assert(unix.process())
assert(process:forkexec(PATH, { lua, "-e", "local unix = require \"dromozoa.unix\" unix.nanosleep(0.2)" }))

assert(unix.unblock_signal(unix.SIGCHLD))
local result, message, code, t = unix.nanosleep(10)
assert(unix.block_signal(unix.SIGCHLD))

if verbose then
  io.stderr:write(message, "\n")
  io.stderr:write(tostring(t), "\n")
end
assert(not result)
assert(code == unix.EINTR)
assert(t > unix.timespec(5))

local pid, reason, status = assert(unix.wait())
assert(pid == process[1])
assert(reason == "exit")
assert(status == 0)

local t1 = assert(unix.clock_gettime(unix.CLOCK_MONOTONIC))
local t2 = assert(unix.clock_gettime(unix.CLOCK_MONOTONIC))
if verbose then
  io.stderr:write(tostring(t1), "\n")
  io.stderr:write(tostring(t2), "\n")
  io.stderr:write(tostring(t2 - t1), "\n")
end
assert(t1 < t2)
