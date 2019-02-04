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

local function check_no_attr(result, message, code)
  if verbose then
    print(message)
  end
  assert(not result)
  assert(code == unix.ENOATTR or code == unix.ENODATA)
end

local path = "test.data"
local link = "test.link"

os.remove(path)
os.remove(link)

assert(io.open(path, "w")):close()
assert(unix.symlink(path, link))

assert(unix.setxattr(path, "user.dromozoa-test", "foo"))

assert(unix.getxattr(path, "user.dromozoa-test") == "foo")
assert(unix.getxattr(link, "user.dromozoa-test") == "foo")
assert(unix.lgetxattr(path, "user.dromozoa-test") == "foo")
check_no_attr(unix.lgetxattr(link, "user.dromozoa-test"))

assert(unix.lsetxattr(link, "user.dromozoa-test", "bar"))

assert(unix.getxattr(path, "user.dromozoa-test") == "foo")
assert(unix.getxattr(link, "user.dromozoa-test") == "foo")
assert(unix.lgetxattr(path, "user.dromozoa-test") == "foo")
assert(unix.lgetxattr(link, "user.dromozoa-test") == "bar")

assert(unix.setxattr(link, "user.dromozoa-test", "baz"))

assert(unix.getxattr(path, "user.dromozoa-test") == "baz")
assert(unix.getxattr(link, "user.dromozoa-test") == "baz")
assert(unix.lgetxattr(path, "user.dromozoa-test") == "baz")
assert(unix.lgetxattr(link, "user.dromozoa-test") == "bar")

assert(unix.lremovexattr(link, "user.dromozoa-test"))

assert(unix.getxattr(path, "user.dromozoa-test") == "baz")
assert(unix.getxattr(link, "user.dromozoa-test") == "baz")
assert(unix.lgetxattr(path, "user.dromozoa-test") == "baz")
check_no_attr(unix.lgetxattr(link, "user.dromozoa-test"))

assert(unix.removexattr(link, "user.dromozoa-test"))

check_no_attr(unix.getxattr(path, "user.dromozoa-test"))
check_no_attr(unix.getxattr(link, "user.dromozoa-test"))
check_no_attr(unix.lgetxattr(path, "user.dromozoa-test"))
check_no_attr(unix.lgetxattr(link, "user.dromozoa-test"))

local function verbose_names(names)
  if verbose then
    for i = 1, #names do
      print(names[i])
    end
  end
end

local names = unix.listxattr(path)
assert(#names == 0)

local names = unix.listxattr(link)
assert(#names == 0)

assert(unix.setxattr(path, "user.dromozoa-test1", "foo"))

local names = unix.listxattr(path)
verbose_names(names)
assert(#names == 1)

local names = unix.listxattr(link)
verbose_names(names)
assert(#names == 1)

assert(unix.setxattr(path, "user.dromozoa-test2", "bar"))
assert(unix.setxattr(path, "user.dromozoa-test3", "baz"))
assert(unix.setxattr(path, "user.dromozoa-test4", "qux"))

local names = unix.listxattr(path)
verbose_names(names)
assert(#names == 4)

local names = unix.listxattr(link)
verbose_names(names)
assert(#names == 4)

local names = unix.llistxattr(path)
verbose_names(names)
assert(#names == 4)

local names = unix.llistxattr(link)
verbose_names(names)
assert(#names == 0)

assert(unix.lsetxattr(link, "user.dromozoa-test5", ("x"):rep(256)))
assert(unix.lsetxattr(link, "user.dromozoa-test6", ("y"):rep(256)))
assert(unix.lsetxattr(link, "user.dromozoa-test7", ("z"):rep(256)))

local names = unix.llistxattr(link)
verbose_names(names)
assert(#names == 3)
