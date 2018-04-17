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

local verbose = os.getenv "VERBOSE" == "1"

assert(unix.ignore_signal(unix.SIGPIPE))

local reader, writer = unix.pipe()
assert(reader:close())

local result, message, code = writer:write "X"
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
assert(code == unix.EPIPE)

assert(unix.kill(unix.getpid(), 0))
assert(unix.kill(-unix.getpgrp(), 0))
