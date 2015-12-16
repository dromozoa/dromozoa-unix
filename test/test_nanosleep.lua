-- Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local nanosleep = require "dromozoa.unix.nanosleep"

assert(nanosleep({ tv_sec = 0, tv_nsec = 200 }) == 0)

local result, message, code, tv = nanosleep({ tv_sec = -1, tv_nsec = 0 })
assert(result == nil)
assert(tv.tv_sec == 0)
assert(tv.tv_nsec == 0)
