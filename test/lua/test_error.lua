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

local verbose = os.getenv "VERBOSE" == "1"

unix.set_errno(0)
if verbose then
  io.stderr:write(unix.strerror(), "\n")
end
assert(unix.get_errno() == 0)

unix.set_errno(unix.EAGAIN)
assert(unix.strerror() == unix.strerror(unix.EAGAIN))
assert(unix.get_errno() == unix.EAGAIN)

if verbose then
  io.stderr:write(unix.strerror(unix.EAGAIN), "\n")
  io.stderr:write(unix.strerror(unix.EINTR), "\n")
end

assert(unix.get_errno() == unix.EAGAIN)

local result, message, code = unix.get_error()
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
assert(message == unix.strerror(unix.EAGAIN))
assert(code == unix.EAGAIN)

assert(unix.get_last_errno() == 0)
assert(not unix.open("no such file", unix.O_RDONLY))
assert(unix.get_last_errno() == unix.ENOENT)

local result, message, code = unix.get_error()
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
assert(message == unix.strerror(unix.ENOENT))
assert(code == unix.ENOENT)

unix.set_errno(unix.EEXIST)
assert(unix.strerror() == unix.strerror(unix.EEXIST))
assert(unix.get_errno() == unix.EEXIST)
