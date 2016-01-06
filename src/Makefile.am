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
	environ.hpp \
	error.hpp \
	fcntl.hpp \
	fd.hpp \
	forkexec.hpp \
	function.hpp \
	gettimeofday.hpp \
	log_level.hpp \
	nanosleep.hpp \
	ndelay.hpp \
	pathexec.hpp \
	pipe.hpp \
	read.hpp \
	set_field.hpp \
	signal.hpp \
	signal_mask.hpp \
	success.hpp \
	wait.hpp \
	write.hpp

luaexec_LTLIBRARIES = unix.la

unix_la_LDFLAGS = -module -avoid-version
unix_la_SOURCES = \
	argument_vector.cpp \
	coe.cpp \
	environ.cpp \
	error.cpp \
	fcntl.cpp \
	fd.cpp \
	forkexec.cpp \
	gettimeofday.cpp \
	log_level.cpp \
	nanosleep.cpp \
	ndelay.cpp \
	pathexec.cpp \
	pipe.cpp \
	read.cpp \
	set_field.cpp \
	signal.cpp \
	signal_mask.cpp \
	success.cpp \
	unix.cpp \
	wait.cpp \
	write.cpp
