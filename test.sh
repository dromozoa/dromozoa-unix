#! /bin/sh -e

# Copyright (C) 2015-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
#
# This file is part of dromozoa-unix.
#
# dromozoa-unix is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dromozoa-unix is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dromozoa-unix.  If not, see <http://www.gnu.org/licenses/>.

LUA_PATH="test/lua/?.lua;;"
export LUA_PATH

for i in test/lua/test*.lua
do
  case X$# in
    X0) lua "$i";;
    *) "$@" "$i";;
  esac
done

rm -fr test.lock test.fifo test.sock test.dir* test.txt* test.data test.link
