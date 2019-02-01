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

local path = "test.data"
local link = "test.link"

os.remove(path)
os.remove(link)

assert(io.open(path, "w")):close()
assert(unix.symlink(path, link))

assert(unix.setxattr(path, "dromozoa-test", "foobarbaz"))
assert(unix.getxattr(path, "dromozoa-test") == "foobarbaz")
assert(unix.lsetxattr(link, "dromozoa-test", "qux"))
assert(unix.lgetxattr(link, "dromozoa-test") == "qux")
assert(unix.getxattr(link, "dromozoa-test") == "foobarbaz")



