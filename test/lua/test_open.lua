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

local path = os.getenv("PATH")
local envp = unix.environ()

local fd = assert(unix.open("test.txt", unix.O_WRONLY + unix.O_CREAT + unix.O_CLOEXEC))
fd:write("foo\n")

local pid = assert(unix.process():forkexec(path, { "ls", "-l" }, envp, "/", { [1] = fd, [2] = fd }))[1]
local a, b, c = unix.wait()
-- print(a, b, c)
assert(a == pid)
assert(b == "exit")
assert(c == 0)

fd:write("bar\n")
fd:close()
