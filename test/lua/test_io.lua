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

assert(unix.ignore_signal(unix.SIGPIPE))

local reader, writer = unix.pipe()

-- unix.set_log_level(3)

assert(writer:write("abc") == 3)
assert(reader:read(3) == "abc")

assert(writer:write("abc", 4) == 0)
assert(writer:write("abc", 3) == 1)
assert(writer:write("abc", 2) == 2)
assert(writer:write("abc", 1) == 3)
assert(writer:write("abc", 0) == 3)
assert(reader:read(9) == "cbcabcabc")

assert(writer:write("abc", -1) == 1)
assert(writer:write("abc", -2) == 2)
assert(writer:write("abc", -3) == 3)
assert(writer:write("abc", -4) == 3)
assert(reader:read(9) == "cbcabcabc")

assert(writer:write("abc", 1, 4) == 3)
assert(writer:write("abc", 1, 3) == 3)
assert(writer:write("abc", 1, 2) == 2)
assert(writer:write("abc", 1, 1) == 1)
assert(writer:write("abc", 1, 0) == 0)
assert(reader:read(9) == "abcabcaba")

assert(writer:write("abc", 1, -1) == 3)
assert(writer:write("abc", 1, -2) == 2)
assert(writer:write("abc", 1, -3) == 1)
assert(writer:write("abc", 1, -4) == 0)
assert(reader:read(6) == "abcaba")

assert(writer:close())
assert(reader:read(1) == "")
assert(reader:close())

local reader, writer = unix.pipe(unix.O_NONBLOCK);

local result = reader:read(1)
assert(result == unix.resource_unavailable_try_again)
assert(reader:close())
local result = writer:write("foo")
assert(result == unix.broken_pipe)
assert(writer:close())
