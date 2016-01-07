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

#include <signal.h>

#include "error.hpp"
#include "function.hpp"
#include "set_field.hpp"
#include "signal.hpp"
#include "signal_mask.hpp"
#include "success.hpp"

namespace dromozoa {
  namespace {
    int impl_kill(lua_State* L) {
      pid_t pid = luaL_checkinteger(L, 1);
      int sig = luaL_optinteger(L, 2, SIGTERM);
      if (kill(pid, sig) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_default_signal(lua_State* L) {
      struct sigaction sa = {};
      sa.sa_handler = SIG_DFL;
      if (sigaction(luaL_checkinteger(L, 1), &sa, 0) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_ignore_signal(lua_State* L) {
      struct sigaction sa = {};
      sa.sa_handler = SIG_IGN;
      if (sigaction(luaL_checkinteger(L, 1), &sa, 0) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_block_signal(lua_State* L) {
      sigset_t mask;
      if (lua_isnoneornil(L, 1)) {
        if (sigfillset(&mask) == -1) {
          return push_error(L);
        }
      } else {
        if (sigemptyset(&mask) == -1) {
          return push_error(L);
        }
        if (sigaddset(&mask, luaL_checkinteger(L, 1)) == -1) {
          return push_error(L);
        }
      }
      if (signal_mask(SIG_BLOCK, &mask, 0) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }

    int impl_unblock_signal(lua_State* L) {
      sigset_t mask;
      if (lua_isnoneornil(L, 1)) {
        if (sigfillset(&mask) == -1) {
          return push_error(L);
        }
      } else {
        if (sigemptyset(&mask) == -1) {
          return push_error(L);
        }
        if (sigaddset(&mask, luaL_checkinteger(L, 1)) == -1) {
          return push_error(L);
        }
      }
      if (signal_mask(SIG_UNBLOCK, &mask, 0) == -1) {
        return push_error(L);
      } else {
        return push_success(L);
      }
    }
  }

  void initialize_signal(lua_State* L) {
    function<impl_kill>::set_field(L, "kill");
    function<impl_default_signal>::set_field(L, "default_signal");
    function<impl_ignore_signal>::set_field(L, "ignore_signal");
    function<impl_block_signal>::set_field(L, "block_signal");
    function<impl_unblock_signal>::set_field(L, "unblock_signal");

    DROMOZOA_SET_FIELD(L, SIGABRT);
    DROMOZOA_SET_FIELD(L, SIGALRM);
    DROMOZOA_SET_FIELD(L, SIGBUS);
    DROMOZOA_SET_FIELD(L, SIGCHLD);
    DROMOZOA_SET_FIELD(L, SIGCONT);
    DROMOZOA_SET_FIELD(L, SIGFPE);
    DROMOZOA_SET_FIELD(L, SIGHUP);
    DROMOZOA_SET_FIELD(L, SIGILL);
    DROMOZOA_SET_FIELD(L, SIGINT);
    DROMOZOA_SET_FIELD(L, SIGKILL);
    DROMOZOA_SET_FIELD(L, SIGPIPE);
    DROMOZOA_SET_FIELD(L, SIGQUIT);
    DROMOZOA_SET_FIELD(L, SIGSEGV);
    DROMOZOA_SET_FIELD(L, SIGSTOP);
    DROMOZOA_SET_FIELD(L, SIGTERM);
    DROMOZOA_SET_FIELD(L, SIGTSTP);
    DROMOZOA_SET_FIELD(L, SIGTTIN);
    DROMOZOA_SET_FIELD(L, SIGTTOU);
    DROMOZOA_SET_FIELD(L, SIGUSR1);
    DROMOZOA_SET_FIELD(L, SIGUSR2);
    DROMOZOA_SET_FIELD(L, SIGSYS);
    DROMOZOA_SET_FIELD(L, SIGTRAP);
    DROMOZOA_SET_FIELD(L, SIGURG);
    DROMOZOA_SET_FIELD(L, SIGVTALRM);
    DROMOZOA_SET_FIELD(L, SIGXCPU);
    DROMOZOA_SET_FIELD(L, SIGXFSZ);
  }
}
