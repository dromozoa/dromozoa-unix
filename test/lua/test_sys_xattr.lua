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

--[[
http://man7.org/linux/man-pages/man7/xattr.7.html

For this reason, extended user attributes are allowed only for
regular files and directories, and access to extended user attributes
is restricted to the owner and to users with appropriate capabilities
for directories with the sticky bit set (see the chmod(1) manual page
for an explanation of the sticky bit).
]]

local sysname = unix.uname().sysname
local test_nofollow = sysname == "Darwin"

if verbose then
  print(sysname, test_nofollow)
end

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

if test_nofollow then
  assert(unix.lgetxattr(path, "user.dromozoa-test") == "foo")
  check_no_attr(unix.lgetxattr(link, "user.dromozoa-test"))

  assert(unix.lsetxattr(link, "user.dromozoa-test", "bar"))

  assert(unix.getxattr(path, "user.dromozoa-test") == "foo")
  assert(unix.getxattr(link, "user.dromozoa-test") == "foo")
  assert(unix.lgetxattr(path, "user.dromozoa-test") == "foo")
  assert(unix.lgetxattr(link, "user.dromozoa-test") == "bar")
end

assert(unix.setxattr(link, "user.dromozoa-test", "baz"))

assert(unix.getxattr(path, "user.dromozoa-test") == "baz")
assert(unix.getxattr(link, "user.dromozoa-test") == "baz")

if test_nofollow then
  assert(unix.lgetxattr(path, "user.dromozoa-test") == "baz")
  assert(unix.lgetxattr(link, "user.dromozoa-test") == "bar")

  assert(unix.lremovexattr(link, "user.dromozoa-test"))

  assert(unix.getxattr(path, "user.dromozoa-test") == "baz")
  assert(unix.getxattr(link, "user.dromozoa-test") == "baz")
  assert(unix.lgetxattr(path, "user.dromozoa-test") == "baz")
  check_no_attr(unix.lgetxattr(link, "user.dromozoa-test"))
end

assert(unix.removexattr(link, "user.dromozoa-test"))

check_no_attr(unix.getxattr(path, "user.dromozoa-test"))
check_no_attr(unix.getxattr(link, "user.dromozoa-test"))

if test_nofollow then
  check_no_attr(unix.lgetxattr(path, "user.dromozoa-test"))
  check_no_attr(unix.lgetxattr(link, "user.dromozoa-test"))
end

local function check_names(names, expect)
  local n = 0
  for i = 1, #names do
    local name = names[i]
    if verbose then
      print(name)
    end
    if name:find "^user%." then
      n = n + 1
    end
  end
  assert(n == expect)
end

check_names(unix.listxattr(path), 0)
check_names(unix.listxattr(link), 0)

assert(unix.setxattr(path, "user.dromozoa-test1", "foo"))

check_names(unix.listxattr(path), 1)
check_names(unix.listxattr(link), 1)

assert(unix.setxattr(path, "user.dromozoa-test2", "bar"))
assert(unix.setxattr(path, "user.dromozoa-test3", "baz"))
assert(unix.setxattr(path, "user.dromozoa-test4", "qux"))

check_names(unix.listxattr(path), 4)
check_names(unix.listxattr(link), 4)

if test_nofollow then
  check_names(unix.llistxattr(path), 4)
  check_names(unix.llistxattr(link), 0)

  assert(unix.lsetxattr(link, "user.dromozoa-test5", ("x"):rep(256)))
  assert(unix.lsetxattr(link, "user.dromozoa-test6", ("y"):rep(256)))
  assert(unix.lsetxattr(link, "user.dromozoa-test7", ("z"):rep(256)))

  check_names(unix.llistxattr(path), 4)
  check_names(unix.llistxattr(link), 3)
end

os.remove(path)
os.remove(link)
