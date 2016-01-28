-- Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local pairs = require "dromozoa.commons.pairs"
local sequence = require "dromozoa.commons.sequence"
local string_buffer = require "dromozoa.commons.string_buffer"

local function timespec_add(t1, t2)
  local t = t2 % 1
  local s = t1.tv_sec + t2 - t
  local n = t1.tv_nsec + t * 1000000000 % 1
  if n >= 1000000000 then
    s = s + 1
    n = n - 1000000000
  end
  return {
    tv_sec = s;
    tv_nsec = n;
  }
end

local function timespec_compare(t1, t2)
  local s1 = t1.tv_sec
  local s2 = t2.tv_sec
  if s1 == s2 then
    return t1.tv_nsec - t2.tv_nsec
  else
    return s1 - s2
  end
end

local class = {}

function class.timespec_now()
  local now = class.super.gettimeofday()
  return {
    tv_sec = now.tv_sec;
    tv_nsec = now.tv_usec * 1000;
  }
end

function class.new(selector)
  return {
    selector = selector;
    pendings = {};
    buffers = {};
  }
end

function class:add(fd)
  self.selector:add(fd, 0)
  self.buffers[fd:get()] = string_buffer()
  return self
end

function class:del(fd)
  self.selector:del(fd)
  self.buffers[fd:get()] = nil
  return self
end

function class:add_pending(fd, event, timeout)
  self.selector:mod(fd, event)
  self.pendings[fd:get()] = {
    coroutine = coroutine.running();
    timeout = timeout;
  }
  return coroutine.yield()
end

function class:read(fd, count, timeout)
  if type(timeout) == "number" then
    timeout = timespec_add(class.timespec_now(), timeout)
  end
  local buffer = self.buffers[fd:get()]
  local result = buffer:read(count)
  if result == nil then
    local result = fd:read(1024)
    if result == class.super.resource_unavailable_try_again then
      if self:add_pending(fd, 1, timeout) == nil then
        return nil
      end
    elseif result == "" then
      buffer:close()
    else
      buffer:write(result)
    end
    return self:read(fd, count, timeout)
  else
    return result
  end
end

function class:stop()
  self.stopped = true
  return self
end

function class:dispatch()
  local selector = self.selector
  local pendings = self.pendings
  local timeout = { tv_sec = 1, tv_nsec = 0 }
  while not self.stopped do
    local result = selector:select(timeout)
    local now = class.timespec_now()
    local resumes = sequence()
    for i = 1, result do
      local fd, event = selector:event(i)
      local pending = pendings[fd]
      if pending ~= nil then
        pendings[fd] = nil
        resumes:push({ pending.coroutine, event })
      end
    end
    for fd, pending in pairs(pendings) do
      local timeout = pending.timeout
      if timeout ~= nil and timespec_compare(timeout, now) <= 0 then
        pendings[fd] = nil
        resumes:push({ pending.coroutine })
      end
    end
    for resume in resumes:each() do
      coroutine.resume(resume[1], resume[2])
    end
  end
end

local metatable = {
  __index = class;
}

return setmetatable(class, {
  __call = function (_, selector)
    return setmetatable(class.new(selector), metatable)
  end;
})
