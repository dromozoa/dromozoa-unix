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

local fd, tmpname = assert(unix.mkstemp("tmp-XXXXXX"))

local current_time = assert(unix.clock_gettime(unix.CLOCK_REALTIME))

local st1 = assert(fd:fstat())
local st2 = assert(unix.stat(tmpname))
assert(equal(st1, st2))
assert(st1.st_size == 0)
assert(st2.st_size == 0)
assert(st1.st_ctim <= current_time)
assert(st1.st_mtim <= current_time)
assert(st2.st_ctim <= current_time)
assert(st2.st_mtim <= current_time)

assert(fd:write("foobarbazqux"))
assert(fd:fsync())

local st1 = assert(fd:fstat())
local st2 = assert(unix.stat(tmpname))
assert(equal(st1, st2))
assert(st1.st_size == 12)
assert(st2.st_size == 12)

fd:close()
assert(os.remove(tmpname))
