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

noinst_HEADERS = \
	argument_vector.hpp \
	coe.hpp \
	error.hpp \
	fcntl.hpp \
	fd.hpp \
	forkexec.hpp \
	lock.hpp \
	ndelay.hpp \
	netdb.hpp \
	netinet.hpp \
	pathexec.hpp \
	pipe.hpp \
	read.hpp \
	selector.hpp \
	selector_epoll.hpp \
	selector_kqueue.hpp \
	selfpipe.hpp \
	signal.hpp \
	signal_mask.hpp \
	sockaddr.hpp \
	socket.hpp \
	stdlib.hpp \
	sys_socket.hpp \
	sys_stat.hpp \
	sys_time.hpp \
	sys_wait.hpp \
	time.hpp \
	unistd.hpp \
	write.hpp

luaexec_LTLIBRARIES = unix.la

unix_la_CPPFLAGS = -I$(top_srcdir)/bind
unix_la_LDFLAGS = -module -avoid-version -shared
unix_la_LIBADD = $(top_srcdir)/bind/libdromozoa-bind.la
unix_la_SOURCES = \
	argument_vector.cpp \
	coe.cpp \
	error.cpp \
	fcntl.cpp \
	fd.cpp \
	forkexec.cpp \
	lock.cpp \
	ndelay.cpp \
	netdb.cpp \
	netinet.cpp \
	pathexec.cpp \
	pipe.cpp \
	read.cpp \
	selector.cpp \
	selfpipe.cpp \
	signal.cpp \
	signal_mask.cpp \
	sockaddr.cpp \
	socket.cpp \
	stdlib.cpp \
	sys_socket.cpp \
	sys_stat.cpp \
	sys_time.cpp \
	sys_wait.cpp \
	time.cpp \
	unistd.cpp \
	write.cpp \
	module.cpp

if HAVE_EPOLL
unix_la_SOURCES += selector_epoll.cpp
endif
if HAVE_KQUEUE
unix_la_SOURCES += selector_kqueue.cpp
endif
