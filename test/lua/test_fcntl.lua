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

local uint32 = require "dromozoa.commons.uint32"
local unix = require "dromozoa.unix"

assert(unix.O_CLOEXEC)
assert(unix.O_NONBLOCK)

os.remove("test.txt")

local fd = assert(unix.open("test.txt", uint32.bor(unix.O_WRONLY, unix.O_CREAT, unix.O_CLOEXEC)))
assert(fd:is_coe())
assert(fd:is_ndelay_off())
fd:write("foo\n")
fd:close()

local fd = assert(unix.open("test.txt"))
assert(fd:is_coe())
assert(fd:is_ndelay_off())
assert(fd:read(4) == "foo\n")
fd:close()
