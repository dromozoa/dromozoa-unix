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

print("cwd", unix.getcwd())
print("uid", unix.getuid())
print("gid", unix.getgid())
print("euid", unix.geteuid())
print("egid", unix.getegid())
print("pid", unix.getpid())
print("pgrp", unix.getpgrp())
print("ppid", unix.getppid())

local mask = unix.umask(18) -- 022
assert(mask == 18)
assert(unix.umask(mask) == 18)
