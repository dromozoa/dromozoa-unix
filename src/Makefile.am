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

noinst_HEADERS = common.hpp pathexec.hpp

luaexec_LTLIBRARIES = unix.la

#fcntl_la_LDFLAGS = -module -avoid-version
#fcntl_la_SOURCES = common.cpp unix_fcntl.cpp
#
#gettimeofday_la_LDFLAGS = -module -avoid-version
#gettimeofday_la_SOURCES = common.cpp unix_gettimeofday.cpp
#
#nanosleep_la_LDFLAGS = -module -avoid-version
#nanosleep_la_SOURCES = common.cpp unix_nanosleep.cpp

unix_la_LDFLAGS = -module -avoid-version
unix_la_SOURCES = \
	coe.cpp \
	fcntl.cpp \
	error.cpp \
	fd.cpp \
	gettimeofday.cpp \
	log_level.cpp \
	nanosleep.cpp \
	ndelay.cpp \
	pipe.cpp \
	set_field.cpp \
	success.cpp \
	unix.cpp
