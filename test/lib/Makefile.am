# Copyright (C) 2016 Tomoyuki Fujimori <moyu@dromozoa.com>
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

TESTS = $(check_PROGRAMS)

noinst_HEADERS = assert.hpp

check_PROGRAMS = \
	test_argument_vector.exe \
	test_async.exe \
	test_compat_clock_gettime.exe \
	test_compat_pipe2.exe \
	test_compat_socket.exe \
	test_compat_strerror.exe \
	test_errno_saver.exe \
	test_forkexec.exe \
	test_pathexec.exe \
	test_selector.exe \
	test_selfpipe.exe \
	test_sigmask.exe \
	test_socket_address.exe

test_argument_vector_exe_CPPFLAGS = -I$(top_srcdir)/include
test_argument_vector_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_argument_vector_exe_SOURCES = test_argument_vector.cpp

test_async_exe_CPPFLAGS = -I$(top_srcdir)/include
test_async_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_async_exe_SOURCES = test_async.cpp

test_compat_clock_gettime_exe_CPPFLAGS = -I$(top_srcdir)/include
test_compat_clock_gettime_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_compat_clock_gettime_exe_SOURCES = test_compat_clock_gettime.cpp

test_compat_pipe2_exe_CPPFLAGS = -I$(top_srcdir)/include
test_compat_pipe2_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_compat_pipe2_exe_SOURCES = test_compat_pipe2.cpp

test_compat_socket_exe_CPPFLAGS = -I$(top_srcdir)/include
test_compat_socket_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_compat_socket_exe_SOURCES = test_compat_socket.cpp

test_compat_strerror_exe_CPPFLAGS = -I$(top_srcdir)/include
test_compat_strerror_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_compat_strerror_exe_SOURCES = test_compat_strerror.cpp

test_errno_saver_exe_CPPFLAGS = -I$(top_srcdir)/include
test_errno_saver_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_errno_saver_exe_SOURCES = test_errno_saver.cpp

test_forkexec_exe_CPPFLAGS = -I$(top_srcdir)/include
test_forkexec_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_forkexec_exe_SOURCES = test_forkexec.cpp

test_pathexec_exe_CPPFLAGS = -I$(top_srcdir)/include
test_pathexec_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_pathexec_exe_SOURCES = test_pathexec.cpp

test_selector_exe_CPPFLAGS = -I$(top_srcdir)/include
test_selector_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_selector_exe_SOURCES = test_selector.cpp

test_selfpipe_exe_CPPFLAGS = -I$(top_srcdir)/include
test_selfpipe_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_selfpipe_exe_SOURCES = test_selfpipe.cpp

test_sigmask_exe_CPPFLAGS = -I$(top_srcdir)/include
test_sigmask_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_sigmask_exe_SOURCES = test_sigmask.cpp

test_socket_address_exe_CPPFLAGS = -I$(top_srcdir)/include
test_socket_address_exe_LDADD = $(top_builddir)/src/lib/libdromozoa-unix.la
test_socket_address_exe_SOURCES = test_socket_address.cpp
