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

local class = {}

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
  local buffer = self.buffers[fd:get()]
  local result = buffer:read(count)
  if result == nil then
    local result = fd:read(1024)
    if result == class.super.EAGAIN or result == class.super.EWOULDBLOCK then
      self:add_pending(fd, 1, timeout)
      return self:read(fd, count, timeout)
    elseif result == "" then
      buffer:close()
      return self:read(fd, count, timeout)
    else
      buffer:write(result)
      return self:read(fd, count, timeout)
    end
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
    print("select", result)
    -- local now = class.super.gettimeofday
    local resumes = sequence()
    for i = 1, result do
      local fd, event = selector:event(i)
      print("fd-event", fd, event)
      local pending = pendings[fd]
      pendings[fd] = nil
      if pending ~= nil then
        resumes:push(pending.coroutine)
      end
    end
    for resume in resumes:each() do
      coroutine.resume(resume)
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
