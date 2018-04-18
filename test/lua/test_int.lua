-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

assert(unix.band(0x01) == 0x01)
assert(unix.band(0x01, 0x03) == 0x01)
assert(unix.band(0x01, 0x03, 0x07) == 0x01)
assert(unix.band(0x1F, 0xF1) == 0x11)

assert(unix.bor(0x01) == 0x01)
assert(unix.bor(0x01, 0x02) == 0x03)
assert(unix.bor(0x01, 0x02, 0x04) == 0x07)
assert(unix.bor(0x1F, 0xF1) == 0xFF)

local result, message = pcall(unix.band, 1, "foo")
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
