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

-- unix.set_log_level(3)

local path = os.getenv("PATH")
local envp = unix.environ()

local selector = unix.selector()
assert(selector:open(256, unix.O_CLOEXEC))
selector:add(unix.selfpipe.get(), 1)

local pid1 = assert(unix.process():forkexec(path, { "sh", "-c", ":" }))[1]
local pid2 = assert(unix.process():forkexec(path, { "sh", "-c", ":" }))[1]
-- print(pid1, pid2)

local n = 0
repeat
  assert(unix.unblock_signal(unix.SIGCHLD))
  local a, b, c = selector:select({ tv_sec = 1, tv_nsec = 0 })
  -- print("select", a, b, c)
  assert(unix.block_signal(unix.SIGCHLD))

  if a and a ~= unix.interrupted then
    if a == 0 then
      -- print("timeout")
    else
      for i = 1, a do
        local fd, ev = selector:event(i)
        -- print("event", fd, ev)
        if fd == unix.selfpipe.get() then
          local result = unix.selfpipe.read()
          -- print("selfpipe", result)
          if result > 0 then
            while true do
              local a, b, c = unix.wait(-1, unix.WNOHANG)
              -- print("wait", a, b, c)
              if a and a > 0 then
                n = n + 1
              else
                break
              end
            end
          end
        end
      end
    end
  end
until n == 2
