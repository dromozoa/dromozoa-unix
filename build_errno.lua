-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local source_filename = "docs/errno.h.html"
local target_filename = "src/lua/errno.cpp"

local out = assert(io.open(target_filename, "w"))

out:write(([[
#include <errno.h>

#include "common.hpp"

namespace dromozoa {
  void initialize_errno(lua_State* L) {
]]):format(url))

local handle = assert(io.open(source_filename))
for line in handle:lines() do
  local name = line:match "<dt>%[(E[%w_]+)%]</dt>"
  if name then
    out:write(([[
#ifdef %s
    luaX_set_field(L, -1, "%s", %s);
#endif
]]):format(name, name, name))
  end
end

out:write([[
  }
}
]])

out:close()
