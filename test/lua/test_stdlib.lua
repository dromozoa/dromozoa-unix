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

assert(unix.realpath(".") == unix.getcwd())
assert(unix.realpath(arg[0]) == unix.getcwd() .. "/test/lua/test_stdlib.lua")

local tmpdir = assert(unix.mkdtemp("tmp-XXXXXX"))
print(tmpdir)
assert(os.remove(tmpdir))

local fd, tmpname = assert(unix.mkstemp("tmp-XXXXXX"))
print(tmpname)
assert(fd:is_coe())
assert(fd:write("foo\n"))
assert(fd:close())

local fd = assert(unix.open(tmpname))
assert(fd:is_coe())
assert(fd:read(4) == "foo\n")
assert(fd:read(4) == "")
assert(fd:close())

assert(os.remove(tmpname))
