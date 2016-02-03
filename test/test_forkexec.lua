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

unix.set_log_level(2)

local reader, writer = unix.pipe(unix.O_CLOEXEC)

local path = os.getenv("PATH")
local envp = unix.environ()

local process = unix.process()
assert(process:forkexec(path, { "ls", "-l" }, envp, "/", { [1] = writer }))
local pid = process[1]
-- print(pid)
writer:close()
while true do
  local result = assert(reader:read(64))
  if #result == 0 then
    break
  end
  -- io.write(result)
end
assert(unix.wait() == pid)
reader:close()

local process = unix.process()
local result, message, code = process:forkexec(path, { "no such command" })
local pid = process[1]
-- print(message, code, pid)
assert(unix.wait() == pid)

local process = unix.process()
assert(process:forkexec_daemon(path, { "sleep", "60" }, envp, "/"))
local pid1, pid2 = process[1], process[2]
-- print(pid1, pid2)
assert(unix.wait() == pid1)
assert(unix.kill(pid2, 0))
assert(unix.kill(pid2))

local process = unix.process()
local result, message, code = process:forkexec_daemon(path, { "no such command" })
local pid1, pid2 = process[1], process[2]
-- print(message, code, pid1, pid2)
assert(unix.wait() == pid1)
