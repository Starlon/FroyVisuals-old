# makefile for ae

# change these to reflect your Lua installation
LUA= /tmp/lhf/lua-5.1.4
LUAINC= $(LUA)/src
LUALIB= $(LUA)/src
LUABIN= $(LUA)/src

# these will probably work if Lua has been installed globally
#LUA= /usr/local
#LUAINC= $(LUA)/include
#LUALIB= $(LUA)/lib
#LUABIN= $(LUA)/bin

# probably no need to change anything below here
CC= gcc
CFLAGS= $(INCS) $(WARN) -O2 $G
WARN= -ansi -pedantic -Wall
INCS= -I$(LUAINC)
LIBS= -L$(LUALIB) -llua -lm

OBJS= ae.o
TEST= test.ae

T=tae

all:	test

test:	tae xy
	./tae < $(TEST)
	./xy

o:	$(OBJS)

tae:	$(OBJS) tae.o
	$(CC) -o $@ $@.o $(OBJS) $(LIBS)

xy:	$(OBJS) xy.o
	$(CC) -o $@ $@.o $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) core core.* tae tae.o xy xy.o

# distribution

FTP= $(HOME)/public/ftp/lua/5.1
D= ae
A= $D.tar.gz
TOTAR=Makefile,README,ae.c,ae.h,tae.c,test.ae,xy.c

distr:	clean
	tar zcvf $A -C .. $D/{$(TOTAR)}
	touch -r $A .stamp
	mv $A $(FTP)

diff:	clean
	tar zxf $(FTP)/$A
	diff $D .

# eof
