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

local json = require "dromozoa.commons.json"
local unix = require "dromozoa.unix"

local priority_min = assert(unix.sched_get_priority_min(unix.SCHED_OTHER))
local priority_max = assert(unix.sched_get_priority_max(unix.SCHED_OTHER))
print(priority_min, priority_max)
assert(unix.sched_yield())

if unix.sched_getscheduler then
  print(unix.SCHED_OTHER)
  print(unix.sched_getscheduler(0))
  print(json.encode(assert(unix.sched_getparam(0))))
  assert(unix.sched_setscheduler(0, unix.SCHED_OTHER, { sched_priority = (priority_min + priority_max) * 0.5 }))
  print(json.encode(assert(unix.sched_getparam(0))))
end

if unix.sched_getaffinity then
  local affinity = assert(unix.sched_getaffinity(0))
  print(json.encode(affinity))
  if unix.sched_setaffinity then
    assert(unix.sched_setaffinity(0, { affinity[1] }))
  end
end

