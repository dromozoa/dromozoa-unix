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
local translate_range = require "dromozoa.commons.translate_range"

local class = {}

local function get_fd(fd)
  return class.super.fd.get(fd)
end

local function translate_timeout(timeout)
  if type(timeout) == "number" then
    local timespec = class.super.timespec
    return timespec.now() + timespec(timeout)
  else
    return timeout
  end
end

function class.new(selector)
  return {
    selector = selector;
    selector_timeout = class.super.timespec(1);
    pendings = {};
    buffers = {};
    buffer_size = 1024;
    waitings = {};
    waiting_ref = 0;
  }
end

function class:add(fd)
  self.selector:add(fd, 0)
  self.buffers[get_fd(fd)] = string_buffer()
  return self
end

function class:del(fd)
  self.selector:del(fd)
  self.buffers[get_fd(fd)] = nil
  return self
end

function class:stop()
  self.stopped = true
  return self
end

function class:add_pending(fd, event, timeout)
  self.selector:mod(fd, event)
  self.pendings[get_fd(fd)] = {
    coroutine = coroutine.running();
    timeout = timeout;
  }
  return coroutine.yield()
end

function class:add_waiting(timeout)
  local ref = self.waiting_ref + 1
  self.waitings[ref] = {
    coroutine = coroutine.running();
    timeout = timeout;
  }
  self.waiting_ref = ref
  return coroutine.yield()
end

function class:read(fd, count, timeout)
  timeout = translate_timeout(timeout)
  local buffer = self.buffers[get_fd(fd)]
  local result = buffer:read(count)
  if result == nil then
    local result = class.super.fd.read(fd, self.buffer_size)
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

function class:write(fd, buffer, timeout, size, i, j)
  timeout = translate_timeout(timeout)
  if size == nil then
    size = 0
  end
  local min, max = translate_range(#buffer, i, j)
  local result = class.super.fd.write(fd, buffer)
  if result == 0 or result == class.super.resource_unavailable_try_again then
    if self:add_pending(fd, 2, timeout) == nil then
      return nil, size
    end
  else
    size = size + result
    min = min + result
    if min > max then
      return true, size
    end
  end
  return self:write(fd, buffer, timeout, size, min, max)
end

function class:wait(timeout)
  timeout = translate_timeout(timeout)
  return self:add_waiting(timeout)
end

function class:dispatch()
  local selector = self.selector
  local pendings = self.pendings
  local waitings = self.waitings
  self.stopped = nil
  while not self.stopped do
    local result = selector:select(self.selector_timeout)
    local now = class.super.timespec.now()
    local resumes = sequence()
    for i = 1, result do
      local fd, event = selector:event(i)
      local pending = pendings[fd]
      if pending ~= nil then
        pendings[fd] = nil
        pending.event = event
        resumes:push(pending)
      end
    end
    for fd, pending in pairs(pendings) do
      local timeout = pending.timeout
      if timeout ~= nil and timeout < now then
        pendings[fd] = nil
        resumes:push(pending)
      end
    end
    for ref, waiting in pairs(waitings) do
      local timeout = waiting.timeout
      if timeout ~= nil and timeout < now then
        waitings[ref] = nil
        resumes:push(waiting)
      end
    end
    for resume in resumes:each() do
      coroutine.resume(resume.coroutine, resume.event)
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
