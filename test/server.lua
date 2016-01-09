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

local path = "\0dromozoa-unix/test.sock"
local server = assert(unix.socket(unix.AF_UNIX, unix.SOCK_STREAM))
local result, message, code = server:bind(unix.sockaddr_un(path))
if not result then
  if code == unix.ENOENT then
    path = "test.sock"
    assert(server:bind(unix.sockaddr_un(path)))
  else
    assert(result, message, code)
  end
end
assert(server:listen())
io.stdout:write(path)
io.stdout:flush()
io.stdout:close()

local fd, sa = server:accept(unix.O_CLOEXEC)
while true do
  local result, message, code = fd:read(4096)
  if result and #result > 0 then
    io.stderr:write(result)
    fd:write("bar\n")
  else
    break
  end
end
assert(fd:close())
assert(server:close())

os.remove("test.sock")
