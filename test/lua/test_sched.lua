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

local equal = require "dromozoa.commons.equal"
local json = require "dromozoa.commons.json"
local unix = require "dromozoa.unix"

assert(unix.sched_yield())
assert(unix.SCHED_OTHER)
assert(unix.SCHED_FIFO)

local other_priority_min = assert(unix.sched_get_priority_min(unix.SCHED_OTHER))
local other_priority_max = assert(unix.sched_get_priority_max(unix.SCHED_OTHER))
print("other_priority", other_priority_min, other_priority_max)

local fifo_priority_min = assert(unix.sched_get_priority_min(unix.SCHED_FIFO))
local fifo_priority_max = assert(unix.sched_get_priority_max(unix.SCHED_FIFO))
print("fifo_priority", fifo_priority_min, fifo_priority_max)

if unix.sched_getscheduler and unix.sched_getparam and unix.sched_setscheduler then
  assert(unix.sched_getscheduler(0) == unix.SCHED_OTHER)
  local param = assert(unix.sched_getparam(0))
  assert(other_priority_min <= param.sched_priority)
  assert(param.sched_priority <= other_priority_max)
  local priority = math.min(fifo_priority_min + 1, fifo_priority_max)
  assert(unix.sched_setscheduler(0, unix.SCHED_FIFO, { sched_priority = priority }))
  local param = assert(unix.sched_getparam(0))
  assert(param.sched_priority == priority)
end

if unix.sched_getaffinity and unix.sched_setaffinity then
  print("affinity", json.encode(unix.sched_getaffinity(0)))
  assert(unix.sched_setaffinity(0, { 0 }))
  assert(equal(unix.sched_getaffinity(0), { 0 }))
end
