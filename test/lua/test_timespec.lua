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

local t = unix.timespec(1.25)
assert(t.tv_sec == 1)
assert(t.tv_nsec == 250000000)

local t1 = assert(unix.clock_gettime(unix.CLOCK_MONOTONIC))
unix.nanosleep(0.2)
local t2 = assert(unix.clock_gettime(unix.CLOCK_MONOTONIC))
assert(t1 == t1)
assert(t1 < t2)

local t = t2 - t1
assert(t1 + t == t2)
assert(t.tv_sec == 0)
assert(100000000 < t.tv_nsec and t.tv_nsec < 300000000)
assert(0.1 < t:tonumber() and t:tonumber() < 0.3)

local t1 = unix.timespec { tv_sec = 1, tv_nsec = 0 }
local t2 = unix.timespec { tv_sec = 1, tv_nsec = 1 }
assert(t1 <= t1)
assert(t1 <= t2)
assert(t2 <= t2)

local t = unix.timespec(1 / 3, unix.TIMESPEC_TYPE_REALTIME)
local l = t:tostring()
local u = t:tostring(true)
assert(l:match "^([^%.]+)" == os.date("%Y-%m-%dT%H:%M:%S", 0))
assert(u:match "^([^%.]+)" == os.date("!%Y-%m-%dT%H:%M:%S", 0))
assert(l == tostring(t))

if verbose then
  io.stderr:write(tostring(unix.timespec(os.time(), unix.CLOCK_REALTIME)), "\n")
  io.stderr:write(tostring(unix.clock_gettime(unix.CLOCK_REALTIME)), "\n")
  io.stderr:write(tostring(unix.clock_gettime(unix.CLOCK_MONOTONIC)), "\n")
  io.stderr:write(tostring(unix.clock_gettime(unix.CLOCK_MONOTONIC_RAW)), "\n")
end

local t = unix.clock_gettime(unix.CLOCK_REALTIME)
assert(t.tv_type == unix.TIMESPEC_TYPE_REALTIME)
local t = t + unix.timespec(0);
assert(t.tv_type == unix.TIMESPEC_TYPE_REALTIME)
local t = t + unix.clock_gettime(unix.CLOCK_MONOTONIC)
assert(t.tv_type == unix.TIMESPEC_TYPE_UNKNOWN)
local t = t - unix.clock_gettime(unix.CLOCK_MONOTONIC_RAW)
assert(t.tv_type == unix.TIMESPEC_TYPE_DURATION)
local t = t - unix.timespec(0, unix.TIMESPEC_TYPE_DURATION)
assert(t.tv_type == unix.TIMESPEC_TYPE_DURATION)
local t = t - unix.clock_gettime(unix.CLOCK_MONOTONIC_RAW)
assert(t.tv_type == unix.TIMESPEC_TYPE_UNKNOWN)

local result, message, code = pcall(unix.timespec, "foo")
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

local t = unix.timespec(42)
local t = t:add(2.5)
local t = t:sub(1.25)
assert(t.tv_sec == 43)
assert(t.tv_nsec == 250000000)
