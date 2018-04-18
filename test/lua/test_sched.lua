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

assert(unix.sched_yield())

local other_priority_min = assert(unix.sched_get_priority_min(unix.SCHED_OTHER))
local other_priority_max = assert(unix.sched_get_priority_max(unix.SCHED_OTHER))
local fifo_priority_min = assert(unix.sched_get_priority_min(unix.SCHED_FIFO))
local fifo_priority_max = assert(unix.sched_get_priority_max(unix.SCHED_FIFO))

if verbose then
  io.stderr:write(([[
other_priority_min | %d
other_priority_max | %d
fifo_priority_min  | %d
fifo_priority_max  | %d
]]):format(
    other_priority_min,
    other_priority_max,
    fifo_priority_min,
    fifo_priority_max))
end

if unix.sched_getscheduler and unix.sched_getparam and unix.sched_setscheduler then
  assert(unix.sched_getscheduler() == unix.SCHED_OTHER)
  local param = assert(unix.sched_getparam())
  if verbose then
    io.stderr:write(param.sched_priority, "\n")
  end
  assert(other_priority_min <= param.sched_priority and param.sched_priority <= other_priority_max)
  local priority = math.min(fifo_priority_min + 1, fifo_priority_max)
  local result, message, code = unix.sched_setscheduler(0, unix.SCHED_FIFO, { sched_priority = priority })
  if result then
    assert(unix.sched_getscheduler() == unix.SCHED_FIFO)
    local param = assert(unix.sched_getparam())
    assert(param.sched_priority == priority)
  elseif verbose then
    io.stderr:write(message, "\n")
  end
end

if unix.sched_getaffinity and unix.sched_setaffinity then
  local affinity = assert(unix.sched_getaffinity())
  if verbose then
    for i = 1, #affinity do
      io.stderr:write(affinity[i], "\n")
    end
  end
  assert(unix.sched_setaffinity(0, { 0 }))
end
