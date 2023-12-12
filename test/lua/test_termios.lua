-- Copyright (C) 2023 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local function write(...)
  if verbose then
    io.stderr:write(...)
  end
end

local fd = assert(unix.open("/dev/tty", unix.bor(unix.O_CLOEXEC, unix.O_RDWR, unix.O_NOCTTY)))

if verbose then
  fd:write "start\n"
end

local t = fd:tcgetattr()
write("iflag: ", t:get_iflag(), "\n")
write("oflag: ", t:get_oflag(), "\n")
write("cflag: ", t:get_cflag(), "\n")
write("lflag: ", t:get_lflag(), "\n")
for i = 0, t.NCCS - 1 do
  write("cc[", i, "]: ", t:get_cc(i), "\n")
end

local ispeed = t:cfgetispeed()
local ospeed = t:cfgetospeed()
write("ispeed: ", ispeed, "\n")
write("ospeed: ", ospeed, "\n")

assert(t:cfsetispeed(4800))
assert(t:cfsetospeed(9600))
assert(t:cfgetispeed() == 4800)
assert(t:cfgetospeed() == 9600)
assert(t:cfsetispeed(ispeed))
assert(t:cfsetospeed(ospeed))

assert(fd:tcsetattr(t.TCSANOW, t))

if verbose then
  fd:write "end\n"
end

fd:close()
