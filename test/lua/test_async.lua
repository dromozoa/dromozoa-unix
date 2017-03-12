-- Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local dumper = require "dromozoa.commons.dumper"
local ipairs = require "dromozoa.commons.ipairs"
local uint32 = require "dromozoa.commons.uint32"
local dyld = require "dromozoa.dyld"
local unix = require "dromozoa.unix"

local symbol = dyld.RTLD_DEFAULT:dlsym("pthread_create")
if not symbol or symbol:is_null() then
  dyld.dlopen("libpthread.so.0", uint32.bor(dyld.RTLD_LAZY, dyld.RTLD_GLOBAL))
end

local service = unix.async_service(1)
local selector = unix.selector()

local info = service:info()
assert(info.spare_threads == 1)
assert(info.current_threads == 1)
assert(info.current_tasks == 0)

assert(selector:add(service:get(), unix.SELECTOR_READ))

local hints = {
  ai_socktype = unix.SOCK_STREAM;
}
assert(service:push(unix.async_getaddrinfo("github.com", "https", hints)))
assert(service:push(unix.async_getaddrinfo("luarocks.org", "https", hints)))
assert(service:push(unix.async_getaddrinfo("www.lua.org", "https", hints)))
assert(service:push(unix.async_getaddrinfo("www.google.com", "https", hints)))
assert(service:push(unix.async_getaddrinfo("test-ipv6.com", "https", hints)))
local count = 5

while true do
  local result = selector:select()
  -- print("select", result)
  for i = 1, result do
    local fd, event = selector:event(i)
    -- print("event", fd, event)
    if fd == service:get() then
      local result = service:read()
      -- print("read", result)
      while true do
        local task = service:pop()
        -- print(task)
        if task then
          local a, b = task:result()
          if type(a) == "table" then
            print(dumper.encode(a))
            for i, ai in ipairs(a) do
              assert(service:push(ai.ai_addr:async_getnameinfo()))
              count = count + 1
            end
          else
            assert(a, b)
          end
          count = count - 1
        else
          break
        end
      end
    end
  end
  if count == 0 then
    break
  end
end

local tasks = {
  unix.async_nanosleep(0.25);
  unix.async_nanosleep(0.25);
  unix.async_nanosleep(0.25);
  unix.async_nanosleep(0.25);
}

for _, task in ipairs(tasks) do
  assert(service:push(task))
end
service:cancel(tasks[2])

while true do
  local task = service:pop()
  if task then
    assert(task == tasks[1])
    break
  else
    unix.nanosleep(0.05)
  end
end
unix.nanosleep(0.05)

assert(service:close())

assert(service:pop() == tasks[3])
assert(not service:pop())

local thread = coroutine.create(function ()
  assert(tasks[1]:result())
  assert(tasks[3]:result())
end)
assert(coroutine.resume(thread))
