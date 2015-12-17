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

local gettimeofday = require "dromozoa.unix.gettimeofday"

local t1 = os.time()
local t2 = gettimeofday()
local t3 = gettimeofday()

assert(os.difftime(t2.tv_sec, t1) < 2)

local u = t3.tv_usec - t2.tv_usec
local s = t3.tv_sec - t2.tv_sec
if u < 0 then
  u = u + 1000000
  s = s - 1
end
assert(s == 0)
-- print(("%d.%06d"):format(s, u))
