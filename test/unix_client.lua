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

local abstract = ...
local fd = assert(unix.socket(unix.AF_UNIX, unix.SOCK_STREAM))
if abstract:find("^1") then
  assert(fd:connect(unix.sockaddr_un("\0dromozoa-unix/test.sock")))
else
  assert(fd:connect(unix.sockaddr_un("test.sock")))
end
assert(fd:write("foo\n") == 4)
assert(fd:shutdown(unix.SHUT_WR))
while true do
  local result, message, code = fd:read(256)
  if result and #result > 0 then
    assert(result == "bar\n")
    -- io.stderr:write(result)
  else
    break
  end
end
assert(fd:close())
