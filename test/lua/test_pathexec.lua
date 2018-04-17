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
local envp = unix.get_environ()
envp[#envp + 1] = "foo=bar"

local reader, writer = assert(unix.pipe())
local process = unix.process()
assert(process:forkexec(PATH, { arg[-1], "test/lua/pathexec.lua" }, envp, nil, { [1] = writer }))
assert(writer:close())

local buffer = {}
while true do
  local data = assert(reader:read(256))
  if data == "" then
    break
  else
    buffer[#buffer + 1] = data
  end
end
assert(reader:close())
if verbose then
  print(("%q"):format(table.concat(buffer)))
end
assert(table.concat(buffer) == "baz\n")

local pid, reason, status = assert(unix.wait())
assert(pid == process[1])
assert(reason == "exit")
assert(status == 0)
