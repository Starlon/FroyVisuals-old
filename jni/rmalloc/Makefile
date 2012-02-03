# ===========================================
# malloc debug library build and testing
# -------------------------------------------
# See license text at end of file.
# ===========================================
CC = cc

# Compiler flags
CFLAGS = -O2 $(CARG) -D__STDC__=1
# Source files
SOURCES = rtest.c rmalloc.c
# Object files
OBJECTS = rtest.o rmalloc.o
# Archive
ARCHIVE = rmalloc.tgz


rtest:	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o rtest

test:	rtest
	./rtest

rmalloc.o: rmalloc.h

depend:
	makedepend -s "# DO NOT DELETE" -- $(CFLAGS) -- $(SOURCES)

clean:
	rm -f $(OBJECTS) rtest $(ARCHIVE) *~

dist:
	tar cvfz $(ARCHIVE) *.[ch] Makefile *.html

# DO NOT DELETE

# =======================================================
# License:	(This is the open source ISC license, see 
#                http://en.wikipedia.org/wiki/ISC_license 
#                for more info)
#
# Copyright © 2010  Andreas M. Rammelt <rammi@hexco.de>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# =======================================================
