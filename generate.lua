-- Copyright (C) 2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local sequence = require "dromozoa.commons.sequence"
local read_file = require "dromozoa.commons.read_file"
local shell = require "dromozoa.commons.shell"

local url = ...

local content = assert(shell.eval("curl -fL " .. shell.quote(url)))
local dl = assert(content:match("<dl compact>(.-)</dl>"))
local names = sequence()
for name in dl:gmatch("<dt>%[(.-)%]</dt>") do
  names:push(name)
  -- if not dd:match("OB XSR") then
  --   local name = assert(dt:match("^%[(.*)%]$"))
  --   print(name)
  -- end
end

local out = assert(io.open("src/lua/errno.cpp", "w"))

out:write(([[
// generated from %s

#include <errno.h>

#include "common.hpp"

namespace dromozoa {
  void initialize_errno(lua_State* L) {
]]):format(url))

for name in names:each() do
  out:write(([[
#ifdef %s
    luaX_set_field(L, -1, "%s", %s);
#endif
]]):format(name, name, name))
end

out:write([[
  }
}
]])

out:close()
