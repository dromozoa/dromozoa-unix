# Copyright (C) 2015 Tomoyuki Fujimori <moyu@dromozoa.com>
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

CPPFLAGS = -I$(LUA_INCDIR)
CXXFLAGS = -Wall -W -Wno-missing-field-initializers $(CFLAGS)
LDFLAGS = -L$(LUA_LIBDIR) $(LIBFLAG)
LDLIBS =

TARGET = gettimeofday.so nanosleep.so

all: $(TARGET)

clean:
	rm -f *.o $(TARGET)

gettimeofday.so: gettimeofday.o common.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

nanosleep.so: nanosleep.o common.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

install:
	mkdir -p $(LIBDIR)/dromozoa/unix
	cp $(TARGET) $(LIBDIR)/dromozoa/unix
