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

local code = unix.get_errno()

unix.set_errno(0)
assert(unix.get_errno() == 0)
print(unix.strerror())

unix.set_errno(unix.ETIMEDOUT)
assert(unix.get_errno() == unix.ETIMEDOUT)
print(unix.strerror())

assert(unix.strerror() == unix.strerror(unix.ETIMEDOUT))

-- print(unix.strerror(unix.EAGAIN))
-- print(unix.strerror(unix.EINPROGRESS))
-- print(unix.strerror(unix.EINTR))
-- print(unix.strerror(unix.ENOENT))
-- print(unix.strerror(unix.EPIPE))
-- print(unix.strerror(unix.EWOULDBLOCK))
