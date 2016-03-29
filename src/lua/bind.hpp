// Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-unix.
//
// dromozoa-unix is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-unix is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-unix.  If not, see <http://www.gnu.org/licenses/>.

#ifndef BIND_HPP
#define BIND_HPP

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <exception>

namespace dromozoa {
  template <lua_CFunction T>
  struct function {
    static int value(lua_State* L) {
      try {
        return T(L);
      } catch (const std::exception& e) {
        return luaL_error(L, "caught: %s", e.what());
      } catch (...) {
        lua_pushliteral(L, "caught");
        return lua_error(L);
      }
    }

    static void set_field(lua_State* L, const char* key) {
      lua_pushcfunction(L, value);
      lua_setfield(L, -2, key);
    }
  };

  template <class T>
  struct push_impl {
    static void push(lua_State* L, lua_Integer value) {
      lua_pushinteger(L, value);
    }
  };

  template <lua_CFunction T>
  struct push_impl<function<T> > {
    template <class U>
    static void push(lua_State* L, const U&) {
      lua_pushcfunction(L, U::value);
    }
  };

  template <>
  struct push_impl<char*> {
    static void push(lua_State* L, const char* value) {
      lua_pushstring(L, value);
    }
  };

  template <class T>
  inline void set_field(lua_State* L, const char* key, const T& value) {
    push_impl<T>::push(L, value);
    lua_setfield(L, -2, key);
  }

  template <class T>
  inline T* new_userdata(lua_State* L) {
    return static_cast<T*>(lua_newuserdata(L, sizeof(T)));
  }

  template <class T>
  inline T* test_userdata(lua_State* L, int n, const char* name) {
    if (void* data = lua_touserdata(L, n)) {
      if (lua_getmetatable(L, n)) {
        luaL_getmetatable(L, name);
        if (!lua_rawequal(L, -1, -2)) {
          data = 0;
        }
        lua_pop(L, 2);
        return static_cast<T*>(data);
      }
    }
    return 0;
  }

  template <class T>
  inline T* check_userdata(lua_State* L, int n, const char* name) {
    return static_cast<T*>(luaL_checkudata(L, n, name));
  }

  template <class T>
  inline T* check_userdata(lua_State* L, int n, const char* name1, const char* name2) {
    if (T* data = test_userdata<T>(L, n, name1)) {
      return data;
    } else {
      return check_userdata<T>(L, n, name2);
    }
  }

  inline void new_metatable(lua_State* L, const char* name) {
    luaL_newmetatable(L, name);
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
  }

  template <class T>
  inline void new_metatable(lua_State* L, const char* name, const T&) {
    luaL_newmetatable(L, name);
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    T::set_field(L, "__gc");
    lua_pop(L, 1);
  }

  inline void set_metatable(lua_State* L, const char* name) {
    luaL_getmetatable(L, name);
    lua_setmetatable(L, -2);
  }

  template <class T>
  inline void set_metafield(lua_State* L, const char* name, const T&) {
    if (!lua_getmetatable(L, -1)) {
      lua_newtable(L);
    }
    T::set_field(L, name);
    lua_setmetatable(L, -2);
  }
}

#define DROMOZOA_SET_FIELD(L, value) \
  dromozoa::set_field(L, #value, (value))

#endif
