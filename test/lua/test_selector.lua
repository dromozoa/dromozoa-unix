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

local reader, writer = assert(unix.pipe(unix.O_CLOEXEC + unix.O_NONBLOCK))

assert(unix.SELECTOR_READ_WRITE == unix.bor(unix.SELECTOR_READ, unix.SELECTOR_WRITE))

local selector = assert(unix.selector())
if verbose then
  io.stderr:write(selector:get(), "\n")
end
assert(selector:get() ~= -1)
assert(selector:add(reader, unix.SELECTOR_READ))

assert(selector:select(0.2) == 0)

assert(writer:write "X")
assert(selector:select() == 1)
assert(reader:read(1) == "X")
local result, message, code = reader:read(1)
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
assert(code == unix.EAGAIN or code == unix.EWOULDBLOCK)

assert(selector:select(0.2) == 0)

assert(writer:close())
assert(selector:select() == 1)
assert(reader:read(1) == "")
assert(reader:close())
