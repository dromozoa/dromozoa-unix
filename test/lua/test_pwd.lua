-- Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local pw = assert(unix.getpwnam "root")
assert(pw.pw_name == "root")
assert(pw.pw_uid == 0)
assert(pw.pw_gid == 0)
if verbose then
  io.write(([[
pw_dir   | %s
pw_shell | %s
]]):format(pw.pw_dir, pw.pw_shell))
end

local pw, message = unix.getpwnam "no such user"
assert(pw == nil)
assert(message == nil)

local pw = assert(unix.getpwuid(0))
assert(pw.pw_name == "root")
assert(pw.pw_uid == 0)
assert(pw.pw_gid == 0)
if verbose then
  io.write(([[
pw_dir   | %s
pw_shell | %s
]]):format(pw.pw_dir, pw.pw_shell))
end

local pw, message = unix.getpwnam(99999999)
assert(pw == nil)
assert(message == nil)
