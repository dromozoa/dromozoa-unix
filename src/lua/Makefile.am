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

luaexec_LTLIBRARIES = unix.la

noinst_HEADERS = common.hpp

unix_la_CPPFLAGS = -I$(top_srcdir)/bind -I$(top_srcdir)/include
unix_la_LDFLAGS = -module -avoid-version -shared
unix_la_LIBADD = $(top_srcdir)/src/lib/libdromozoa-unix.la
unix_la_SOURCES = \
	argument_vector.cpp \
	async_service.cpp \
	async_task.cpp \
	errno.cpp \
	error.cpp \
	fcntl.cpp \
	fd.cpp \
	fd_djb.cpp \
	fd_socket.cpp \
	fd_unistd.cpp \
	module.cpp \
	netdb.cpp \
	netinet.cpp \
	pathexec.cpp \
	pipe.cpp \
	process.cpp \
	sched.cpp \
	selector.cpp \
	selfpipe.cpp \
	signal.cpp \
	sockaddr.cpp \
	stdlib.cpp \
	sys_mman.cpp \
	sys_socket.cpp \
	sys_stat.cpp \
	sys_statvfs.cpp \
	sys_wait.cpp \
	time.cpp \
	timer.cpp \
	timespec.cpp \
	unistd.cpp
