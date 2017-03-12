-- Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local sequence = require "dromozoa.commons.sequence"
local unix = require "dromozoa.unix"

for i in sequence.each(assert(unix.get_environ())) do
  local k, v = i:match("([^=]+)=(.*)")
  assert(os.getenv(k) == v)
end

assert(unix.getcwd() == unix.realpath("."))

local tmpdir = assert(unix.mkdtemp("tmp-XXXXXX"))
assert(unix.chdir(tmpdir))
assert(os.remove(unix.getcwd()))

local a, b, c = unix.getcwd()
assert(a == nil)
assert(c == unix.ENOENT)

assert(unix.getuid() > 0)
assert(unix.getgid() > 0)
assert(unix.geteuid() > 0)
assert(unix.getegid() > 0)
assert(unix.getpid() > 0)
assert(unix.getpgrp() > 0)
assert(unix.getppid() > 0)

assert(unix.sysconf(unix["_SC_NPROCESSORS_CONF"]))
assert(unix.sysconf(unix["_SC_NPROCESSORS_ONLN"]))
assert(unix.hardware_concurrency())
