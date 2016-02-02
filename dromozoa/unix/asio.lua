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
local queue = require "dromozoa.commons.queue"
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

local function add_pending(self, fd, event, timeout)
  self.selector:mod(fd, event)
  self.pendings[get_fd(fd)] = {
    coroutine = coroutine.running();
    timeout = timeout;
  }
  return coroutine.yield()
end

local function add_waiting(self, timeout)
  local ref = self.waiting_ref + 1
  self.waitings[ref] = {
    coroutine = coroutine.running();
    timeout = timeout;
  }
  self.waiting_ref = ref
  return coroutine.yield()
end

local function timedout()
  local unix = class.super
  local code = unix.ETIMEDOUT
  return nil, unix.strerror(code), code
end

function class.new(selector)
  return {
    selector = selector;
    selector_timeout = class.super.timespec(1);
    pendings = {};
    buffers = {};
    buffer_size = 1024;
    writtens = {};
    waitings = {};
    waiting_ref = 0;
    resumes = queue();
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

function class:accept(fd, flags, timeout)
  timeout = translate_timeout(timeout)
  local a, b, c = class.super.fd.accept(fd, flags)
  if a == class.super.resource_unavailable_try_again then
    if add_pending(self, fd, 1, timeout) == nil then
      return timedout()
    end
    return self:accept(fd, flags, timeout)
  elseif a == nil then
    return a, b, c
  else
    return a
  end
end

function class:connect(fd, address, timeout)
  timeout = translate_timeout(timeout)
  local a, b, c = class.super.fd.connect(fd, address)
  if a == class.super.operation_in_progress then
    if add_pending(self, fd, 2, timeout) == nil then
      return timedout()
    end
    local unix = self.super
    local code = unix.fd.getsockopt(fd, unix.SOL_SOCKET, unix.SO_ERROR)
    if code == 0 then
      return fd
    else
      if unix.get_raise_error() then
        error(unix.strerror(code))
      else
        return nil, unix.strerror(code), code
      end
    end
  elseif a == nil then
    return a, b, c
  else
    return a
  end
end

function class:read(fd, count, timeout)
  timeout = translate_timeout(timeout)
  local buffer = self.buffers[get_fd(fd)]
  local result = buffer:read(count)
  if result == nil then
    local a, b, c = class.super.fd.read(fd, self.buffer_size)
    if a == class.super.resource_unavailable_try_again then
      if add_pending(self, fd, 1, timeout) == nil then
        return timedout()
      end
    elseif a == nil then
      return a, b, c
    elseif a == "" then
      buffer:close()
    else
      buffer:write(a)
    end
    return self:read(fd, count, timeout)
  else
    return result
  end
end

function class:read_line(fd, delimiter, timeout)
  timeout = translate_timeout(timeout)
  local buffer = self.buffers[get_fd(fd)]
  local line, char = buffer:read_line(delimiter)
  if line == nil then
    local a, b, c = class.super.fd.read(fd, self.buffer_size)
    if a == class.super.resource_unavailable_try_again then
      if add_pending(self, fd, 1, timeout) == nil then
        return nil
      end
    elseif a == nil then
      return a, b, c
    elseif a == "" then
      buffer:close()
    else
      buffer:write(a)
    end
    return self:read_line(fd, delimiter, timeout)
  else
    return line, char
  end
end

function class:write(fd, buffer, timeout, size, i, j)
  timeout = translate_timeout(timeout)
  if size == nil then
    size = 0
  end
  local min, max = translate_range(#buffer, i, j)
  local a, b, c = class.super.fd.write(fd, buffer)
  if a == 0 or a == class.super.resource_unavailable_try_again then
    if add_pending(self, fd, 2, timeout) == nil then
      self.writtens[get_fd(fd)] = size
      return timedout()
    end
  elseif a == nil then
    self.writtens[get_fd(fd)] = size
    return a, b, c
  else
    size = size + a
    min = min + a
    if min > max then
      self.writtens[get_fd(fd)] = size
      return fd
    end
  end
  return self:write(fd, buffer, timeout, size, min, max)
end

function class:written(fd)
  return self.writtens[get_fd(fd)]
end

function class:wait(timeout)
  timeout = translate_timeout(timeout)
  return add_waiting(self, timeout)
end

function class:resume(f)
  return self.resumes:push({
    coroutine = coroutine.create(f);
  })
end

function class:error(message, level)
  return error(message, level)
end

function class:dispatch()
  local selector = self.selector
  local pendings = self.pendings
  local waitings = self.waitings
  local resumes = self.resumes
  self.stopped = nil
  while not self.stopped do
    while true do
      local resume = resumes:front()
      if resume == nil then
        break
      else
        resumes:pop()
        local result, message = coroutine.resume(resume.coroutine, resume.event)
        if not result then
          return self:error(message, 2)
        end
      end
    end
    local result = selector:select(self.selector_timeout)
    local now = class.super.timespec.now()
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
      if timeout == nil or timeout < now then
        waitings[ref] = nil
        resumes:push(waiting)
      end
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
