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

assert(unix.block_signal())
assert(unix.ignore_signal(unix.SIGPIPE))
assert(unix.selfpipe.get() == -1)
assert(unix.selfpipe.install())
assert(unix.selfpipe.get() ~= -1)

local path = os.getenv("PATH")
local envp = unix.environ()

local pid1 = unix.forkexec(path, { "echo", "foo" }, envp, "/", {})
local pid2 = unix.forkexec(path, { "echo", "bar" }, envp, "/", {})

print(pid1, pid2)

local n = 0
repeat
  assert(unix.unblock_signal(unix.SIGCHLD))
  local count = unix.selfpipe.count()
  assert(unix.block_signal(unix.SIGCHLD))
  if count > 0 then
    while true do
      local a, b, c = unix.wait(-1, unix.WNOHANG)
      print(a, b, c)
      if a and a > 0 then
        n = n + 1
      else
        break
      end
    end
  end
until n == 2
