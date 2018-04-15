-- Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local dyld = require "dromozoa.dyld"
local unix = require "dromozoa.unix"

assert(dyld.dlopen_pthread())

local service = unix.async_service(8)

local hints = {
  ai_socktype = unix.SOCK_STREAM;
}

local tasks = {
  unix.async_getaddrinfo("github.com", "https", hints);
  unix.async_getaddrinfo("luarocks.org", "https", hints);
  unix.async_getaddrinfo("www.lua.org", "https", hints);
  unix.async_getaddrinfo("www.google.com", "https", hints);
  unix.async_getaddrinfo("test-ipv6.com", "https", hints);
}

for _, task in ipairs(tasks) do
  assert(service:push(task))
end

local count = 5
while true do
  local task = service:pop()
  if task then
    count = count - 1
    if count == 0 then
      break
    end
  else
    unix.nanosleep(0.05)
  end
end

assert(service:close())
