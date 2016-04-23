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

assert(unix.block_signal(unix.SIGCHLD))
assert(unix.selfpipe.get() == -1)
assert(unix.selfpipe.open())
assert(unix.selfpipe.get() ~= -1)

local PATH = os.getenv("PATH")

local selector = assert(unix.selector(1024))
assert(selector:add(unix.selfpipe.get(), unix.SELECTOR_READ))

local process1 = assert(unix.process())
local process2 = assert(unix.process())
local process3 = assert(unix.process())
local process4 = assert(unix.process())
assert(process1:forkexec(PATH, { "echo", "foo" }))
assert(process2:forkexec(PATH, { "echo", "bar" }))
assert(process3:forkexec(PATH, { "echo", "baz" }))
assert(process4:forkexec(PATH, { "echo", "qux" }))

local n = 0
repeat
  assert(unix.unblock_signal(unix.SIGCHLD))
  local a, b, c = selector:select(1)
  assert(unix.block_signal(unix.SIGCHLD))
  if a == nil then
    assert(c == unix.EINTR, b)
    print(b)
  else
    assert(a == 1)
    assert(unix.selfpipe.read() > 0)
    while true do
      local a, b, c = unix.wait(-1, unix.WNOHANG)
      if a == nil then
        assert(c == unix.ECHILD, b)
        print(b)
        break
      elseif a == 0 then
        break
      else
        n = n + 1
      end
    end
  end
until n == 4
