// Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#ifndef DROMOZOA_FORKEXEC_HPP
#define DROMOZOA_FORKEXEC_HPP

#include <sys/types.h>

namespace dromozoa {
  int forkexec(const char* path, const char* const* argv, const char* const* envp, const char* chdir, const int* dup2_stdio, pid_t& pid);
  int forkexec_daemon(const char* path, const char* const* argv, const char* const* envp, const char* chdir, pid_t& pid1, pid_t& pid2);
}

#endif
