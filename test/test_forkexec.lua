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

local reader, writer = unix.pipe(unix.O_CLOEXEC)

local pid = unix.forkexec(os.getenv("PATH"), { "ls", "-l" }, unix.environ(), "/", { [1] = writer })
writer:close()
while true do
  local result = assert(reader:read(64))
  if #result == 0 then
    break
  end
  -- io.write(result)
end
assert(unix.wait() == pid)

local t1 = unix.gettimeofday()
local pid = unix.forkexec(os.getenv("PATH"), { "sleep", "1" }, unix.environ(), "/", {})
assert(unix.wait() == pid)
local t2 = unix.gettimeofday()

local u = t2.tv_usec - t1.tv_usec
local s = t2.tv_sec - t1.tv_sec
if u < 0 then
  u = u + 1000000
  s = s - 1
end
-- print(("%d.%06d"):format(s, u))

assert(not unix.forkexec(os.getenv("PATH"), { "no such command" }, unix.environ(), "/", {}))

local pid = unix.forkexec_daemon(os.getenv("PATH"), { "sleep", "60" }, unix.environ(), "/")
print(pid)
