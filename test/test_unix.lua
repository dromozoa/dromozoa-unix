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

assert(unix.selfpipe.install())

local path = os.getenv("PATH")
local envp = unix.environ()

local reader, writer = unix.pipe(unix.O_CLOEXEC)

assert(unix.block_signal(unix.SIGCHLD))
local pid1 = assert(unix.forkexec(path, { arg[-1], "test/server.lua" }, envp, nil, { [1] = writer }))
local unix_path = assert(reader:read(256))
print(("%q"):format(unix_path))

local pid2 = assert(unix.forkexec(path, { arg[-1], "test/client.lua", unix_path }, envp, nil, {}))

assert(unix.unblock_signal(unix.SIGCHLD))
unix.selfpipe.read()
assert(unix.block_signal(unix.SIGCHLD))

print(unix.wait())
print(unix.wait())

assert(unix.selfpipe.uninstall())
