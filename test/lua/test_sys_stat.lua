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

assert(unix.umask(tonumber("022", 8)))

local mask = assert(unix.umask(tonumber("022", 8)))
assert(mask == tonumber("022", 8))
assert(unix.umask(mask) == tonumber("022", 8))

assert(unix.mkdir "test.dir")
local st = assert(unix.stat "test.dir")
assert(st.st_mode == unix.bor(unix.S_IFDIR, tonumber("0755", 8)))

assert(os.remove "test.dir")

assert(unix.mkdir("test.dir", tonumber("0707", 8)))
local st = assert(unix.stat "test.dir")
assert(st.st_mode == unix.bor(unix.S_IFDIR, tonumber("0705", 8)))

assert(os.remove "test.dir")

assert(unix.mkfifo "test.fifo")
local st = assert(unix.stat "test.fifo")
assert(st.st_mode == unix.bor(unix.S_IFIFO, tonumber("0644", 8)))

assert(os.remove "test.fifo")

assert(unix.UTIME_NOW)
assert(unix.UTIME_OMIT)
if verbose then
  io.write(([[
UTIME_NOW  | %d
UTIME_OMIT | %d
]]):format(unix.UTIME_NOW, unix.UTIME_OMIT))
end

assert(io.open("test.txt", "w")):close()
assert(unix.chmod("test.txt", tonumber("0666", 8)))
local st = assert(unix.stat "test.txt")
assert(st.st_mode == unix.bor(unix.S_IFREG, tonumber("0666", 8)))
assert(unix.chmod("test.txt", tonumber("0600", 8)))
local st = assert(unix.stat "test.txt")
assert(st.st_mode == unix.bor(unix.S_IFREG, tonumber("0600", 8)))

local t = unix.clock_gettime(unix.TIMESPEC_TYPE_REALTIME)
t = t - 1 -- hack
if verbose then
  io.write(("t: %s\n"):format(tostring(t)))
end

assert(unix.utimensat(unix.AT_FDCWD, "test.txt", 0, 0))
local st = assert(unix.stat "test.txt")
if verbose then
  io.write(([[
st_atim: %s
st_mtim: %s
]]):format(tostring(st.st_atim), tostring(st.st_mtim)))
end
assert(st.st_atim == unix.timespec(0))
assert(st.st_mtim == unix.timespec(0))

assert(unix.utimensat(unix.AT_FDCWD, "test.txt"))
local st = assert(unix.stat "test.txt")
if verbose then
  io.write(([[
st_atim: %s
st_mtim: %s
]]):format(tostring(st.st_atim), tostring(st.st_mtim)))
end
assert(st.st_atim > t)
assert(st.st_mtim > t)

assert(unix.utimensat(unix.AT_FDCWD, "test.txt", nil, 0))
local st = assert(unix.stat "test.txt")
if verbose then
  io.write(([[
st_atim: %s
st_mtim: %s
]]):format(tostring(st.st_atim), tostring(st.st_mtim)))
end
assert(st.st_atim > t)
assert(st.st_mtim == unix.timespec(0))

assert(unix.utimensat(unix.AT_FDCWD, "test.txt", 0, nil))
local st = assert(unix.stat "test.txt")
if verbose then
  io.write(([[
st_atim: %s
st_mtim: %s
]]):format(tostring(st.st_atim), tostring(st.st_mtim)))
end
assert(st.st_atim == unix.timespec(0))
assert(st.st_mtim > t)

assert(os.remove "test.txt")

assert(unix.mknod("test.fifo", unix.bor(unix.S_IFIFO, tonumber("0644", 8)), 0))
local st = assert(unix.stat "test.fifo")
assert(st.st_mode == unix.bor(unix.S_IFIFO, tonumber("0644", 8)))

assert(os.remove "test.fifo")
