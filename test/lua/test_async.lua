-- Copyright (C) 2016-2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local verbose = os.getenv "VERBOSE" == "1"

assert(dyld.dlopen_pthread())

local service = assert(unix.async_service(1))
local selector = assert(unix.selector())
local timer = unix.timer()

local info = service:info()
assert(info.spare_threads == 1)
assert(info.current_threads == 1)
assert(info.current_tasks == 0)

assert(selector:add(service:get(), unix.SELECTOR_READ))

timer:start()

local serv = "https"
local hints = { ai_family = unix.AF_INET, ai_socktype = unix.SOCK_STREAM }
local tasks = {
  unix.async_getaddrinfo("honoka.dromozoa.com", serv, hints);
  unix.async_getaddrinfo("kotori.dromozoa.com", serv, hints);
  unix.async_getaddrinfo("hanayo.dromozoa.com", serv, hints);
  unix.async_getaddrinfo("nozomi.dromozoa.com", serv, hints);
}
for i = 1, #tasks do
  local task = tasks[i]
  if verbose then
    io.stderr:write(tostring(task), "\n")
    io.stderr:flush()
  end
  assert(service:push(task))
end
local n = #tasks

repeat
  local result = assert(selector:select())
  if verbose then
    io.stderr:write(result, "\n")
    io.stderr:flush()
  end
  assert(result == 1)
  local fd, event = assert(selector:event(1))
  assert(fd == service:get())
  assert(event == unix.SELECTOR_READ)
  assert(service:read() == 1)
  while true do
    local task = service:pop()
    if task then
      local index
      local a, b = assert(task:result())
      if type(a) == "table" then
        if verbose then
          io.stderr:write(tostring(task), "\n")
          io.stderr:flush()
        end
        local addrinfo = a
        for i = 1, #addrinfo do
          assert(service:push(addrinfo[i].ai_addr:async_getnameinfo()))
          n = n + 1
        end
      else
        local host, serv = a, b
        if verbose then
          io.stderr:write(host, "\n")
          io.stderr:write(serv, "\n")
          io.stderr:flush()
        end
        assert(host:find "%.dromozoa%.com$")
        assert(serv == "https")
      end
      n = n - 1
    else
      break
    end
  end
until n == 0

timer:stop()
if verbose then
  io.stderr:write(timer:elapsed(), "\n")
end

local tasks = {
  unix.async_nanosleep(0.25);
  unix.async_nanosleep(0.25);
  unix.async_nanosleep(0.25);
  unix.async_nanosleep(0.25);
}
for i = 1, #tasks do
  assert(service:push(tasks[i]))
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
