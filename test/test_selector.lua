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
unix.set_raise_error(true)
unix.selfpipe.install()
unix.block_signal(unix.SIGCHLD)

local PATH = os.getenv("PATH")

local selector = unix.selector():open(1, unix.O_CLOEXEC)
selector:add(unix.selfpipe.get(), 1)

local pid = assert(unix.process():forkexec(PATH, { "sleep", "1" }))[1]
assert(unix.wait(-1, unix.WNOHANG) == 0)
unix.unblock_signal(unix.SIGCHLD)
assert("select", selector:select(unix.timespec(3)) == unix.interrupted)
assert("select", selector:select(unix.timespec(3)) == 1)
unix.block_signal(unix.SIGCHLD)
assert("selfpipe", unix.selfpipe.read() == 1)
assert(unix.wait(-1, unix.WNOHANG) == pid)

selector:del(unix.selfpipe.get())
selector:close()
unix.selfpipe.uninstall()
