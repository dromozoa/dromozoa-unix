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

-- unix.set_log_level(3)

local fd1, fd2 = assert(unix.socketpair(unix.AF_UNIX, unix.SOCK_STREAM))
assert(fd1:coe())
assert(fd2:coe())

local sa1 = fd1:getsockname()
assert(sa1:family() == unix.AF_UNIX)
assert(sa1:path() == "")
-- print(sa1:size())
-- print(("%q"):format(sa1:path()))
local sa2 = fd1:getpeername()
assert(sa2:family() == unix.AF_UNIX)
assert(sa1:path())
-- print(sa2:size())
-- print(("%q"):format(sa2:path()))

assert(fd1:getsockopt(unix.SOL_SOCKET, unix.SO_ERROR) == 0)
assert(fd1:getsockopt(unix.SOL_SOCKET, unix.SO_RCVBUF) > 0);
assert(fd1:getsockopt(unix.SOL_SOCKET, unix.SO_SNDBUF) > 0);

fd1:write("foo")
fd1:close()

assert(fd2:read(4) == "foo")
assert(fd2:read(4) == "")
fd2:close()
