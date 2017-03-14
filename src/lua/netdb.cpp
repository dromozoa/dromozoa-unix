// Copyright (C) 2016,2017 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <netdb.h>

#include <vector>

#include "common.hpp"

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifndef NI_MAXSERV
#define NI_MAXSERV 32
#endif

namespace dromozoa {
  namespace {
    void push_netdb_error(lua_State* L, int code) {
      luaX_push(L, luaX_nil);
      if (const char* what = gai_strerror(code)) {
        luaX_push(L, what);
      } else {
        lua_pushfstring(L, "error number %d", code);
      }
      luaX_push(L, code);
    }

    template <class T>
    class optional {
    public:
      optional() : initialized_() {}

      template <class U>
      explicit optional(const U& value) : initialized_(true), value_(value) {}

      const T* get() const {
        if (initialized_) {
          return &value_;
        } else {
          return 0;
        }
      }

    private:
      bool initialized_;
      T value_;
    };

    template <class T>
    inline optional<T> make_optional() {
      return optional<T>();
    }

    template <class T>
    inline optional<T> make_optional(const T& value) {
      return optional<T>(value);
    }

    optional<struct addrinfo> check_hints(lua_State* L, int arg) {
      if (lua_isnoneornil(L, arg)) {
        return make_optional<struct addrinfo>();
      } else {
        struct addrinfo hints = {};
        hints.ai_flags = luaX_opt_integer_field<int>(L, arg, "ai_flags", AI_V4MAPPED | AI_ADDRCONFIG);
        hints.ai_family = luaX_opt_integer_field<int>(L, arg, "ai_family", AF_UNSPEC);
        hints.ai_socktype = luaX_opt_integer_field<int>(L, arg, "ai_socktype", 0);
        hints.ai_protocol = luaX_opt_integer_field<int>(L, arg, "ai_protocol", 0);
        return make_optional(hints);
      }
    }

    void new_result(lua_State* L, const struct addrinfo* result) {
      lua_newtable(L);
      int i = 1;
      for (const struct addrinfo* ai = result; ai; ai = ai->ai_next, ++i) {
        lua_newtable(L);
        luaX_set_field(L, -1, "ai_family", ai->ai_family);
        luaX_set_field(L, -1, "ai_socktype", ai->ai_socktype);
        luaX_set_field(L, -1, "ai_protocol", ai->ai_protocol);
        luaX_set_field(L, -1, "ai_addrlen", ai->ai_addrlen);
        if (ai->ai_addr) {
          new_sockaddr(L, ai->ai_addr, ai->ai_addrlen);
          luaX_set_field(L, -2, "ai_addr");
        }
        if (ai->ai_canonname) {
          luaX_set_field(L, -1, "ai_canonname", ai->ai_canonname);
        }
        luaX_set_field(L, -2, i);
      }
    }

    void impl_getaddrinfo(lua_State* L) {
      const char* nodename = lua_tostring(L, 1);
      const char* servname = lua_tostring(L, 2);
      optional<struct addrinfo> hints = check_hints(L, 3);
      struct addrinfo* result = 0;
      int code = getaddrinfo(nodename, servname, hints.get(), &result);
      if (code == 0) {
        new_result(L, result);
        freeaddrinfo(result);
      } else {
        push_netdb_error(L, code);
      }
    }

    void impl_getnameinfo(lua_State* L) {
      const socket_address* address = check_sockaddr(L, 1);
      int flags = luaX_opt_integer<int>(L, 2, 0);
      std::vector<char> nodename(NI_MAXHOST);
      std::vector<char> servname(NI_MAXSERV);
      int code = getnameinfo(address->get(), address->size(), &nodename[0], nodename.size(), &servname[0], servname.size(), flags);
      if (code == 0) {
        luaX_push(L, &nodename[0]);
        luaX_push(L, &servname[0]);
      } else {
        push_netdb_error(L, code);
      }
    }

    class async_getaddrinfo : public async_task_impl {
    public:
      async_getaddrinfo(const char* nodename, const char* servname, const optional<struct addrinfo>& hints) : hints_(hints), result_(), code_() {
        if (nodename) {
          nodename_ = make_optional<std::string>(nodename);
        }
        if (servname) {
          servname_ = make_optional<std::string>(servname);
        }
      }

      ~async_getaddrinfo() {
        if (result_) {
          freeaddrinfo(result_);
        }
      }

      virtual void dispatch() {
        const char* nodename = nodename_.get() ? nodename_.get()->c_str() : 0;
        const char* servname = servname_.get() ? servname_.get()->c_str() : 0;
        code_ = getaddrinfo(nodename, servname, hints_.get(), &result_);
      }

      virtual void impl_result(lua_State* L) {
        if (result_) {
          new_result(L, result_);
        } else {
          push_netdb_error(L, code_);
        }
      }

    private:
      optional<std::string> nodename_;
      optional<std::string> servname_;
      optional<struct addrinfo> hints_;
      struct addrinfo* result_;
      int code_;
      async_getaddrinfo(const async_getaddrinfo&);
      async_getaddrinfo& operator=(const async_getaddrinfo&);
    };

    void impl_async_getaddrinfo(lua_State* L) {
      const char* nodename = lua_tostring(L, 1);
      const char* servname = lua_tostring(L, 2);
      optional<struct addrinfo> hints = check_hints(L, 3);
      luaX_new<async_getaddrinfo>(L, nodename, servname, hints);
      luaX_set_metatable(L, "dromozoa.unix.async_task");
    }

    class async_getnameinfo : public async_task_impl {
    public:
      async_getnameinfo(const socket_address& address, int flags) : address_(address), nodename_(NI_MAXHOST), servname_(NI_MAXSERV), flags_(flags) {}

      virtual void dispatch() {
        code_ = getnameinfo(address_.get(), address_.size(), &nodename_[0], nodename_.size(), &servname_[0], servname_.size(), flags_);
      }

      virtual void impl_result(lua_State* L) {
        if (code_ == 0) {
          luaX_push(L, &nodename_[0]);
          luaX_push(L, &servname_[0]);
        } else {
          push_netdb_error(L, code_);
        }
      }

    private:
      socket_address address_;
      std::vector<char> nodename_;
      std::vector<char> servname_;
      int flags_;
      int code_;
      async_getnameinfo(const async_getnameinfo&);
      async_getnameinfo& operator=(const async_getnameinfo&);
    };

    void impl_async_getnameinfo(lua_State* L) {
      const socket_address* address = check_sockaddr(L, 1);
      int flags = luaX_opt_integer<int>(L, 2, 0);
      luaX_new<async_getnameinfo>(L, *address, flags);
      luaX_set_metatable(L, "dromozoa.unix.async_task");
    }
  }

  void initialize_netdb(lua_State* L) {
    luaX_set_field(L, -1, "getaddrinfo", impl_getaddrinfo);
    luaX_set_field(L, -1, "async_getaddrinfo", impl_async_getaddrinfo);

    luaX_set_field(L, -1, "AI_PASSIVE", AI_PASSIVE);
    luaX_set_field(L, -1, "AI_CANONNAME", AI_CANONNAME);
    luaX_set_field(L, -1, "AI_NUMERICHOST", AI_NUMERICHOST);
    luaX_set_field(L, -1, "AI_NUMERICSERV", AI_NUMERICSERV);
    luaX_set_field(L, -1, "AI_V4MAPPED", AI_V4MAPPED);
    luaX_set_field(L, -1, "AI_ALL", AI_ALL);
    luaX_set_field(L, -1, "AI_ADDRCONFIG", AI_ADDRCONFIG);

    luaX_set_field(L, -1, "NI_NOFQDN", NI_NOFQDN);
    luaX_set_field(L, -1, "NI_NUMERICHOST", NI_NUMERICHOST);
    luaX_set_field(L, -1, "NI_NAMEREQD", NI_NAMEREQD);
    luaX_set_field(L, -1, "NI_NUMERICSERV", NI_NUMERICSERV);
#ifdef NI_NUMERICSCOPE
    luaX_set_field(L, -1, "NI_NUMERICSCOPE", NI_NUMERICSCOPE);
#endif
    luaX_set_field(L, -1, "NI_DGRAM", NI_DGRAM);
  }

  void initialize_sockaddr_netdb(lua_State* L) {
    luaX_set_field(L, -1, "getnameinfo", impl_getnameinfo);
    luaX_set_field(L, -1, "async_getnameinfo", impl_async_getnameinfo);
  }
}
