-- Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local verbose = os.getenv "VERBOSE" == "1"

local environ = assert(unix.get_environ())
for i = 1, #environ do
  local item = environ[i]
  if verbose then
    io.stderr:write(item, "\n")
  end
  local k, v = assert(item:match "([^=]+)=(.*)")
  assert(os.getenv(k) == v)
end

local here = unix.getcwd()
assert(here == unix.realpath ".")

local tmpdir = assert(unix.mkdtemp "test.dir-XXXXXX")
assert(unix.chdir(tmpdir))
assert(os.remove(unix.getcwd()))

local result, message, code = unix.getcwd()
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
assert(code == unix.ENOENT)

assert(unix.chdir(here))

assert(unix.getuid()  >= 0)
assert(unix.getgid()  >= 0)
assert(unix.geteuid() >= 0)
assert(unix.getegid() >= 0)
assert(unix.getpid()  >= 0)
assert(unix.getpgrp() >= 0)
assert(unix.getppid() >= 0)
if verbose then
  io.stderr:write(([[
uid  | %d
gid  | %d
euid | %d
egid | %d
pid  | %d
pgrp | %d
ppid | %d
]]):format(
    unix.getuid(),
    unix.getgid(),
    unix.geteuid(),
    unix.getegid(),
    unix.getpid(),
    unix.getpgrp(),
    unix.getppid()))
end

assert(unix.sysconf(unix["_SC_NPROCESSORS_CONF"]))
assert(unix.sysconf(unix["_SC_NPROCESSORS_ONLN"]))
assert(unix.hardware_concurrency())
if verbose then
  io.stderr:write(([[
_SC_NPROCESSORS_CONF | %d
_SC_NPROCESSORS_ONLN | %d
hardware_concurrency | %d
]]):format(
    unix.sysconf(unix["_SC_NPROCESSORS_CONF"]),
    unix.sysconf(unix["_SC_NPROCESSORS_ONLN"]),
    unix.hardware_concurrency()))
end
