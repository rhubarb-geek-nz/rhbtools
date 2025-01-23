#
#  Copyright 2008, Roger Brown
#
#  This file is part of Roger Brown's Toolkit.
#
#  This program is free software: you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by the
#  Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
# 
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#
#  $Id: socket.mak 1 2014-02-06 21:56:41Z rhubarb-geek-nz $

include $(MAKEDEFS)

PARTNAME=socket
TARGET=$(OUTDIR_BIN)/$(PARTNAME)$(EXESUFFIX)
OBJS=$(INTDIR)/$(PARTNAME).o 

all: $(TARGET) 

clean:
	$(CLEAN) $(OBJS) $(TARGET)

$(INTDIR)/$(PARTNAME).o: ../src/$(PARTNAME).c
	$(CC_EXE) $(STDOPT) $(STDINCL) -c ../src/$(PARTNAME).c -o $@

$(TARGET): $(OBJS)
	$(LINKAPP) $(LINKAPP_HEAD) $(OBJS) \
		-o $@ \
		$(SOCKLIBS) $(STDLIB)  \
		$(LINKAPP_TAIL)

dist install:

