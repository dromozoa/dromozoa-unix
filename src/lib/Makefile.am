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

noinst_LTLIBRARIES = libdromozoa-unix.la

libdromozoa_unix_la_CPPFLAGS = -I$(top_srcdir)/bind -I$(top_srcdir)/include
libdromozoa_unix_la_LDFLAGS = -static
libdromozoa_unix_la_SOURCES = \
	argument_vector.cpp \
	coe.cpp \
	compat_accept4.cpp \
	compat_clock_gettime.cpp \
	compat_mkostemp.cpp \
	compat_pipe2.cpp \
	compat_sigmask.cpp \
	compat_socket.cpp \
	compat_strerror.cpp \
	errno_saver.cpp \
	file_descriptor.cpp \
	forkexec.cpp \
	lock.cpp \
	ndelay.cpp \
	pathexec.cpp \
	selector.cpp \
	selfpipe.cpp \
	sigmask.cpp \
	socket_address.cpp \
	timer.cpp

if HAVE_EPOLL
libdromozoa_unix_la_SOURCES += selector_epoll.cpp
else
if HAVE_KQUEUE
libdromozoa_unix_la_SOURCES += selector_kqueue.cpp
endif
endif
