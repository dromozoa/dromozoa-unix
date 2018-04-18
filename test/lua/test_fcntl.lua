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

os.remove "test.txt"

local fd = assert(unix.open("test.txt", unix.bor(unix.O_WRONLY, unix.O_CREAT, unix.O_CLOEXEC)))
assert(fd:is_coe())
assert(fd:is_ndelay_off())
assert(fd:write "foo\n" == 4)
assert(fd:close())

local fd = assert(unix.open "test.txt")
local st = assert(fd:fstat())
assert(st.st_mode == unix.bor(unix.S_IFREG, tonumber("0644", 8)))
assert(fd:is_coe())
assert(fd:is_ndelay_off())
assert(fd:read(4) == "foo\n")
assert(fd:close())

assert(os.remove "test.txt")

local fd = assert(unix.open("test.txt", unix.bor(unix.O_WRONLY, unix.O_CREAT, unix.O_CLOEXEC), tonumber("0606", 8)))
local st = assert(fd:fstat())
assert(st.st_mode == unix.bor(unix.S_IFREG, tonumber("0604", 8)))
assert(fd:close())

assert(os.remove "test.txt")
