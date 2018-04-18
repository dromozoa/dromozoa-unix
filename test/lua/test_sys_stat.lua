-- Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

assert(unix.umask(tonumber("022", 8)))

local mask = assert(unix.umask(tonumber("022", 8)))
assert(mask == tonumber("022", 8))
assert(unix.umask(mask) == tonumber("022", 8))

assert(unix.mkdir "test.dir")
local st = assert(unix.stat "test.dir")
assert(st.st_mode == unix.bor(unix.S_IFDIR, tonumber("0755", 8)))

assert(os.remove "test.dir")

assert(unix.mkdir("test.dir", tonumber("0707", 8)))
local st = assert(unix.stat "test.dir")
assert(st.st_mode == unix.bor(unix.S_IFDIR, tonumber("0705", 8)))

assert(os.remove "test.dir")

assert(unix.mkfifo "test.fifo")
local st = assert(unix.stat "test.fifo")
assert(st.st_mode == unix.bor(unix.S_IFIFO, tonumber("0644", 8)))

assert(os.remove "test.fifo")
