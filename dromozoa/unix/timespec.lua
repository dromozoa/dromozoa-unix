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

local class = {}

function class.new(that)
  local t = type(that)
  if t == "number" then
    local a = that % 1
    local b = a * 1000000000
    return {
      tv_sec = that - a;
      tv_nsec = b - b % 1;
    }
  elseif t == "table" then
    if that.tv_nsec == nil then
      return {
        tv_sec = that.tv_sec;
        tv_nsec = that.tv_usec * 1000;
      }
    else
      return that
    end
  else
    return {
      tv_sec = 0;
      tv_nsec = 0;
    }
  end
end

function class.now()
  return class(class.super.gettimeofday())
end

function class:tostring(utc)
  if utc then
    return os.date("!%Y-%m-%dT%H:%M:%S", self.tv_sec) .. (".%09d"):format(self.tv_nsec)
  else
    return os.date("%Y-%m-%dT%H:%M:%S", self.tv_sec) .. (".%09d"):format(self.tv_nsec)
  end
end

function class:tonumber()
  return self.tv_sec + self.tv_nsec * 0.000000001
end

local metatable = {
  __index = class;
}

function metatable:__add(that)
  local s = self.tv_sec + that.tv_sec
  local n = self.tv_nsec + that.tv_nsec
  if n >= 1000000000 then
    s = s + 1
    n = n - 1000000000
  end
  return class({
    tv_sec = s;
    tv_nsec = n;
  })
end

function metatable:__sub(that)
  local s = self.tv_sec - that.tv_sec
  local n = self.tv_nsec - that.tv_nsec
  if n < 0 then
    s = s - 1
    n = n + 1000000000
  end
  return class({
    tv_sec = s;
    tv_nsec = n;
  })
end

function metatable:__eq(that)
  return self.tv_sec == that.tv_sec and self.tv_nsec == that.tv_nsec
end

function metatable:__lt(that)
  local s1 = self.tv_sec
  local s2 = that.tv_sec
  if s1 == s2 then
    return self.tv_nsec < that.tv_nsec
  else
    return s1 < s2
  end
end

function metatable:__le(that)
  local s1 = self.tv_sec
  local s2 = that.tv_sec
  if s1 == s2 then
    return self.tv_nsec <= that.tv_nsec
  else
    return s1 <= s2
  end
end

function metatable:__tostring()
  return self:tostring()
end

return setmetatable(class, {
  __call = function (_, that)
    return setmetatable(class.new(that), metatable)
  end;
})
