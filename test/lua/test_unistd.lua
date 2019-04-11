-- Copyright (C) 2016-2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

if verbose then
  io.write(([[
F_OK | %d
R_OK | %d
W_OK | %d
X_OK | %d
]]):format(unix.F_OK, unix.R_OK, unix.W_OK, unix.X_OK))
end

local result, message, code = unix.access("no such file", unix.F_OK);
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
assert(code == unix.ENOENT)
assert(unix.access(unix.getcwd(), unix.bor(unix.R_OK, unix.W_OK, unix.X_OK)))

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

local result, message, code = unix.chown("no such file", 0, 0)
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
assert(code == unix.ENOENT)

assert(io.open("test.txt", "w")):close()
local handle = assert(io.open("test.txt"))
assert(handle:read "*a" == "")
handle:close()

assert(unix.truncate("test.txt", 8))
local handle = assert(io.open("test.txt"))
assert(handle:read "*a" == "\0\0\0\0\0\0\0\0")
handle:close()

assert(unix.truncate("test.txt", 4))
local handle = assert(io.open("test.txt"))
assert(handle:read "*a" == "\0\0\0\0")
handle:close()

local fd = assert(unix.open("test.txt", unix.bor(unix.O_RDWR, unix.O_CLOEXEC)))
assert(fd:lseek(0, unix.SEEK_CUR) == 0)
assert(fd:write "ab")
assert(fd:lseek(0, unix.SEEK_CUR) == 2)
assert(fd:lseek(4, unix.SEEK_SET) == 4)
assert(fd:write "ef")
assert(fd:lseek(-5, unix.SEEK_END) == 1)
assert(fd:read(4) == "b\0\0e")
assert(fd:lseek(-3, unix.SEEK_CUR) == 2)
assert(fd:write "cd")
assert(fd:close())

local handle = assert(io.open("test.txt"))
assert(handle:read "*a" == "abcdef")
handle:close()

os.remove "test.txt"

assert(unix.sysconf(unix["_SC_GETPW_R_SIZE_MAX"]))
assert(unix.sysconf(unix["_SC_GETGR_R_SIZE_MAX"]))
assert(unix.hardware_concurrency())
if verbose then
  io.stderr:write(([[
_SC_GETPW_R_SIZE_MAX | %d
_SC_GETGR_R_SIZE_MAX | %d
]]):format(
    unix.sysconf(unix["_SC_GETPW_R_SIZE_MAX"]),
    unix.sysconf(unix["_SC_GETGR_R_SIZE_MAX"])))
end
