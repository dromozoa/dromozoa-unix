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

local mode = ...

local fd = assert(unix.open("test.lock", unix.bor(unix.O_WRONLY, unix.O_CREAT, unix.O_CLOEXEC)))
if mode == "1" then
  assert(io.read(1) == "X")
  local result, message, code = fd:lock_exnb()
  if verbose then
    io.stderr:write(message, "\n")
  end
  assert(not result)
  assert(code == unix.EWOULDBLOCK)
  io.write "X"
  io.flush()
  assert(fd:lock_ex())
else
  assert(fd:lock_exnb())
  io.write "X"
  io.flush()
  assert(io.read(1) == "X")
  assert(unix.nanosleep(0.2))
  assert(fd:lock_un())
end
