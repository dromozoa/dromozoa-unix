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

local server = assert(unix.socket(unix.AF_UNIX, unix.SOCK_STREAM + unix.SOCK_CLOEXEC))
os.remove("test.sock")
assert(server:bind(unix.sockaddr_un("test.sock")))
assert(server:listen())
unix.stdout:close()

local fd, sa = assert(server:accept())
assert(fd:is_coe())
assert(fd:read(1) == "x")
assert(fd:read(1) == "")
assert(fd:write("x"))
assert(fd:close())
assert(server:close())
