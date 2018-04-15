# Copyright (C) 2016,2018 Tomoyuki Fujimori <moyu@dromozoa.com>
#
# This file is part of dromozoa-unix.
#
# dromozoa-unix is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dromozoa-unix is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dromozoa-unix.  If not, see <http://www.gnu.org/licenses/>.

m4_include([version.m4])
AC_INIT([dromozoa-unix], DROMOZOA_UNIX_VERSION, [moyu@dromozoa.com], [], [https://github.com/dromozoa/dromozoa-unix/])
AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile src/lib/Makefile src/lua/Makefile test/lib/Makefile])

AC_PROG_CXX
AC_CHECK_FUNCS([accept4])
AC_CHECK_FUNCS([epoll_create1])
AC_CHECK_FUNCS([epoll_create])
AC_CHECK_FUNCS([kqueue])
AC_CHECK_FUNCS([mach_absolute_time])
AC_CHECK_FUNCS([mkostemp])
AC_CHECK_FUNCS([pipe2])
AC_CHECK_FUNCS([sched_getaffinity])
AC_CHECK_FUNCS([sched_getparam])
AC_CHECK_FUNCS([sched_getscheduler])
AC_CHECK_FUNCS([sched_setaffinity])
AC_CHECK_FUNCS([sched_setscheduler])
AC_CHECK_FUNCS([sysctlbyname])

AC_CHECK_MEMBERS([struct stat.st_atim])
AC_CHECK_MEMBERS([struct stat.st_atimespec])
AC_CHECK_MEMBERS([struct stat.st_ctim])
AC_CHECK_MEMBERS([struct stat.st_ctimespec])
AC_CHECK_MEMBERS([struct stat.st_mtim])
AC_CHECK_MEMBERS([struct stat.st_mtimespec])

AC_SEARCH_LIBS([clock_gettime], [rt], [AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [Define to 1 if you have the 'clock_gettime' function.])])

AX_PTHREAD([], [AC_MSG_ERROR([could not find pthread])])
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
LIBS="$LIBS $PTHREAD_LIBS"
AC_SEARCH_LIBS([pthread_sigmask], [pthread])

AX_PROG_LUA([5.1], [], [], [AC_MSG_ERROR([could not find lua])])
AX_LUA_HEADERS([], [AC_MSG_ERROR([could not find lua])])
AX_LUA_LIBS([], [AC_MSG_ERROR([could not find lua])])

AM_CONDITIONAL([HAVE_EPOLL], [test "x$ac_cv_func_epoll_create" = xyes || test "x$ac_cv_func_epoll_create1" = xyes])
AM_CONDITIONAL([HAVE_KQUEUE], [test "x$ac_cv_func_kqueue" = xyes])

AC_OUTPUT
