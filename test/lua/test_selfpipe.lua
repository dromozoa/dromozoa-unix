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

local PATH = os.getenv "PATH"
local verbose = os.getenv "VERBOSE" == "1"

local selfpipe = assert(unix.selfpipe())
local selector = assert(unix.selector())
assert(selector:add(selfpipe:get(), unix.SELECTOR_READ))

local function test()
  assert(unix.block_signal(unix.SIGCHLD))

  local processes = {
    assert(unix.process());
    assert(unix.process());
    assert(unix.process());
    assert(unix.process());
  }
  assert(processes[1]:forkexec(PATH, { "echo", "foo" }))
  assert(processes[2]:forkexec(PATH, { "echo", "bar" }))
  assert(processes[3]:forkexec(PATH, { "echo", "baz" }))
  assert(processes[4]:forkexec(PATH, { "echo", "qux" }))

  local n = #processes
  repeat
    assert(unix.unblock_signal(unix.SIGCHLD))
    local result, message, code = selector:select(1)
    assert(unix.block_signal(unix.SIGCHLD))
    if not result then
      if verbose then
        io.stderr:write(message, "\n")
      end
      assert(code == unix.EINTR)
    else
      assert(result == 1)
      assert(selfpipe:read() > 0)
      for i = 1, 4 do
        local process = processes[i]
        if process then
          local result = assert(unix.wait(process[1], unix.WNOHANG))
          if result == process[1] then
            processes[i] = nil
            n = n - 1
          end
        end
      end
    end
  until n == 0
end

test()

return test
