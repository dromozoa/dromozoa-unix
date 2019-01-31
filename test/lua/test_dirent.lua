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

local dir = assert(unix.opendir "m4")
local map = {}

while true do
  local loc = assert(dir:telldir())
  local result, message, code = dir:readdir()
  if not result then
    assert(not message)
    assert(not code)
    break
  end
  if verbose then
    print("d_ino", result.d_ino)
    print("d_name", result.d_name)
    print("loc", loc)
  end
  map[result.d_name] = loc
end

assert(map["."])
assert(map[".."])
assert(map["ax_lua.m4"])
local loc = assert(map["ax_pthread.m4"])
assert(dir:seekdir(loc):readdir().d_name == "ax_pthread.m4")

assert(dir:rewinddir())

assert(dir:closedir())
