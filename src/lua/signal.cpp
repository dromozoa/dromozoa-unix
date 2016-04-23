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

#include <dromozoa/compat_sigmask.hpp>
#include <dromozoa/sigmask.hpp>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_kill(lua_State* L) {
      pid_t pid = luaX_check_integer<pid_t>(L, 1);
      int sig = luaX_opt_integer<int>(L, 2, SIGTERM);
      if (kill(pid, sig) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_ignore_signal(lua_State* L) {
      struct sigaction sa = {};
      sa.sa_handler = SIG_IGN;
      if (sigaction(luaX_check_integer<int>(L, 1), &sa, 0) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_block_all_signals(lua_State* L) {
      if (sigmask_block_all_signals(0) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_unblock_all_signals(lua_State* L) {
      if (sigmask_unblock_all_signals(0) == -1) {
        push_error(L);
      } else {
        luaX_push_success(L);
      }
    }

    void impl_block_signal(lua_State* L) {
      int sig = luaX_check_integer<int>(L, 1);
      sigset_t mask;
      if (sigemptyset(&mask) == -1) {
        push_error(L);
      } else {
        if (sigaddset(&mask, sig) == -1) {
          push_error(L);
        } else {
          if (compat_sigmask(SIG_BLOCK, &mask, 0) == -1) {
            push_error(L);
          } else {
            luaX_push_success(L);
          }
        }
      }
    }

    void impl_unblock_signal(lua_State* L) {
      int sig = luaX_check_integer<int>(L, 1);
      sigset_t mask;
      if (sigemptyset(&mask) == -1) {
        push_error(L);
        return;
      } else {
        if (sigaddset(&mask, sig) == -1) {
          push_error(L);
        } else {
          if (compat_sigmask(SIG_UNBLOCK, &mask, 0) == -1) {
            push_error(L);
          } else {
            luaX_push_success(L);
          }
        }
      }
    }
  }

  void initialize_signal(lua_State* L) {
    luaX_set_field(L, -1, "kill", impl_kill);
    luaX_set_field(L, -1, "ignore_signal", impl_ignore_signal);
    luaX_set_field(L, -1, "block_all_signals", impl_block_all_signals);
    luaX_set_field(L, -1, "unblock_all_signals", impl_unblock_all_signals);
    luaX_set_field(L, -1, "block_signal", impl_block_signal);
    luaX_set_field(L, -1, "unblock_signal", impl_unblock_signal);

    luaX_set_field(L, -1, "SIGABRT", SIGABRT);
    luaX_set_field(L, -1, "SIGALRM", SIGALRM);
    luaX_set_field(L, -1, "SIGBUS", SIGBUS);
    luaX_set_field(L, -1, "SIGCHLD", SIGCHLD);
    luaX_set_field(L, -1, "SIGCONT", SIGCONT);
    luaX_set_field(L, -1, "SIGFPE", SIGFPE);
    luaX_set_field(L, -1, "SIGHUP", SIGHUP);
    luaX_set_field(L, -1, "SIGILL", SIGILL);
    luaX_set_field(L, -1, "SIGINT", SIGINT);
    luaX_set_field(L, -1, "SIGKILL", SIGKILL);
    luaX_set_field(L, -1, "SIGPIPE", SIGPIPE);
    luaX_set_field(L, -1, "SIGQUIT", SIGQUIT);
    luaX_set_field(L, -1, "SIGSEGV", SIGSEGV);
    luaX_set_field(L, -1, "SIGSTOP", SIGSTOP);
    luaX_set_field(L, -1, "SIGTERM", SIGTERM);
    luaX_set_field(L, -1, "SIGTSTP", SIGTSTP);
    luaX_set_field(L, -1, "SIGTTIN", SIGTTIN);
    luaX_set_field(L, -1, "SIGTTOU", SIGTTOU);
    luaX_set_field(L, -1, "SIGUSR1", SIGUSR1);
    luaX_set_field(L, -1, "SIGUSR2", SIGUSR2);
    luaX_set_field(L, -1, "SIGSYS", SIGSYS);
    luaX_set_field(L, -1, "SIGTRAP", SIGTRAP);
    luaX_set_field(L, -1, "SIGURG", SIGURG);
    luaX_set_field(L, -1, "SIGVTALRM", SIGVTALRM);
    luaX_set_field(L, -1, "SIGXCPU", SIGXCPU);
    luaX_set_field(L, -1, "SIGXFSZ", SIGXFSZ);
  }
}
