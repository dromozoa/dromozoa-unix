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

assert(unix.block_signal(unix.SIGCHLD))
assert(unix.selfpipe.open())

local PATH = os.getenv("PATH")

assert(unix.nanosleep(0.2))

print(unix.TIMESPEC_TYPE_REALTIME)
print(unix.TIMESPEC_TYPE_MONOTONIC)
print(unix.TIMESPEC_TYPE_DURATION)
print(unix.TIMESPEC_TYPE_UNKNOWN)

local a, b, c, d = unix.nanosleep(-1)
assert(a == nil)
assert(d == unix.timespec(0))
print(b)

local process = assert(unix.process())
assert(process:forkexec(PATH, { arg[-1], "-e", "local unix = require \"dromozoa.unix\" unix.nanosleep(0.2)" }))

assert(unix.unblock_signal(unix.SIGCHLD))
local a, b, c, d = unix.nanosleep(10)
assert(unix.block_signal(unix.SIGCHLD))

assert(a == nil)
assert(c == unix.EINTR)
assert(getmetatable(d))
assert(d > unix.timespec(5))
print(b)
print(d:tonumber())

local a, b, c = assert(unix.wait())
assert(a == process[1])
assert(b == "exit")
assert(c == 0)

local t1 = assert(unix.clock_gettime(unix.CLOCK_MONOTONIC))
local t2 = assert(unix.clock_gettime(unix.CLOCK_MONOTONIC))
assert(t1 < t2)

assert(unix.selfpipe.close())
