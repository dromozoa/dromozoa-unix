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
local PATH = os.getenv "PATH"

local lua
if _G["dromozoa.bind.driver"] then
  lua = "lua"
else
  lua = arg[-1]
end

os.remove "test.lock"

local reader1, writer1 = assert(unix.pipe())
local reader2, writer2 = assert(unix.pipe())
local process1 = unix.process()
local process2 = unix.process()
assert(process1:forkexec(PATH, { lua, "test/lua/lock.lua", "1" }, nil, nil, { [0] = reader1, [1] = writer2 }))
assert(process2:forkexec(PATH, { lua, "test/lua/lock.lua", "2" }, nil, nil, { [0] = reader2, [1] = writer1 }))
assert(reader1:close())
assert(reader2:close())
assert(writer1:close())
assert(writer2:close())

if verbose then
  io.stderr:write(process1[1], "\n")
  io.stderr:write(process2[1], "\n")
end

local pid, reason, status = assert(unix.wait())
assert(pid == process1[1] or process2[1])
assert(reason == "exit")
assert(status == 0)

local pid, reason, status = assert(unix.wait())
assert(pid == process1[1] or process2[1])
assert(reason == "exit")
assert(status == 0)

assert(os.remove "test.lock")
