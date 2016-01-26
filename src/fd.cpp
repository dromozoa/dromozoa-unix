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

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <unistd.h>

#include <iostream>

#include "dromozoa/bind.hpp"

#include "error.hpp"
#include "fd.hpp"

namespace dromozoa {
  using bind::function;
  using bind::get_log_level;
  using bind::push_success;

  namespace {
    struct file_descriptor {
      int fd;
      bool ref;
    };
  }

  int new_fd(lua_State* L, int fd, bool ref) {
    file_descriptor* data = static_cast<file_descriptor*>(lua_newuserdata(L, sizeof(file_descriptor)));
    data->fd = fd;
    data->ref = ref;
    luaL_getmetatable(L, "dromozoa.unix.fd");
    lua_setmetatable(L, -2);
    if (get_log_level() > 2) {
      if (ref) {
        std::cerr << "[dromozoa-unix] new fd-ref " << fd << std::endl;
      } else {
        std::cerr << "[dromozoa-unix] new fd " << fd << std::endl;
      }
    }
    return 1;
  }

  int get_fd(lua_State* L, int n) {
    if (lua_isuserdata(L, n)) {
      return static_cast<const file_descriptor*>(luaL_checkudata(L, n, "dromozoa.unix.fd"))->fd;
    } else {
      return luaL_checkinteger(L, n);
    }
  }

  namespace {
    int impl_new(lua_State* L) {
      int fd = get_fd(L, 2);
      bool ref = lua_toboolean(L, 3);
      return new_fd(L, fd, ref);
    }

    int impl_get(lua_State* L) {
      lua_pushinteger(L, get_fd(L, 1));
      return 1;
    }

    int impl_close(lua_State* L) {
      int fd;
      if (lua_isuserdata(L, 1)) {
        file_descriptor& data = *static_cast<file_descriptor*>(luaL_checkudata(L, 1, "dromozoa.unix.fd"));
        fd = data.fd;
        data.fd = -1;
      } else {
        fd = luaL_checkinteger(L, 1);
      }
      if (fd != -1) {
        if (close(fd) == -1) {
          return push_error(L);
        } else {
          if (get_log_level() > 2) {
            std::cerr << "[dromozoa-unix] close fd " << fd << std::endl;
          }
          return push_success(L);
        }
      } else {
        return push_success(L);
      }
    }

    int impl_gc(lua_State* L) {
      file_descriptor& data = *static_cast<file_descriptor*>(luaL_checkudata(L, 1, "dromozoa.unix.fd"));
      if (data.ref) {
        int fd = data.fd;
        data.fd = -1;
        if (get_log_level() > 2) {
          std::cerr << "[dromozoa-unix] ignore ref-fd " << fd << std::endl;
        }
      } else {
        int fd = data.fd;
        data.fd = -1;
        if (fd != -1) {
          if (get_log_level() > 1) {
            std::cerr << "[dromozoa-unix] fd " << fd << " detected" << std::endl;
          }
          if (close(fd) == -1) {
            int code = errno;
            if (get_log_level() > 0) {
              std::cerr << "[dromozoa-unix] cannot close fd " << fd << ": ";
              print_error(std::cerr, code);
              std::cerr << std::endl;
            }
          } else {
            if (get_log_level() > 2) {
              std::cerr << "[dromozoa-unix] close fd " << fd << std::endl;
            }
          }
        }
      }
      return 0;
    }
  }

  int open_fd(lua_State* L) {
    lua_newtable(L);
    function<impl_get>::set_field(L, "get");
    function<impl_close>::set_field(L, "close");
    lua_newtable(L);
    function<impl_new>::set_field(L, "__call");
    lua_setmetatable(L, -2);
    luaL_newmetatable(L, "dromozoa.unix.fd");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    function<impl_gc>::set_field(L, "__gc");
    lua_pop(L, 1);
    new_fd(L, 0, true);
    lua_setfield(L, -2, "stdin");
    new_fd(L, 1, true);
    lua_setfield(L, -2, "stdout");
    new_fd(L, 2, true);
    lua_setfield(L, -2, "stderr");
    return 1;
  }
}
