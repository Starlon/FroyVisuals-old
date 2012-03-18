# makefile for visscript-lua

# change these to reflect your Lua installation
LUA= /tmp/lhf/lua-5.1.4
LUAINC= $(LUA)/src
LUALIB= $(LUA)/src
LUABIN= $(LUA)/src

# these will probably work if Lua has been installed globally
LUA= ../
LUAINC= $(LUA)../../include/
LUALIB= $(LUA)/x86
LUABIN= $(LUA)/bin

# probably no need to change anything below here
CC= gcc
CFLAGS= $(INCS) $(WARN) -O2 $G
WARN= -Wall
INCS= -I$(LUAINC)
LIBS= -L$(LUALIB) -lluajit -lm -ldl

OBJS= visscript-lua.o
TEST= test.visscript-lua

T=tvisscript-lua

all:	test

test:	xy
	#./tvisscript-lua < $(TEST)
	./xy

o:	$(OBJS)

tvisscript-lua:	$(OBJS) tvisscript-lua.o
	$(CC) -o $@ $@.o $(OBJS) $(LIBS)

xy:	$(OBJS) xy.o
	$(CC) -o $@ $@.o $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) core core.* tvisscript-lua tvisscript-lua.o xy xy.o

# distribution

FTP= $(HOME)/public/ftp/lua/5.1
D= visscript-lua
A= $D.tar.gz
TOTAR=Makefile,README,visscript-lua.c,visscript-lua.h,tvisscript-lua.c,test.visscript-lua,xy.c

distr:	clean
	tar zcvf $A -C .. $D/{$(TOTAR)}
	touch -r $A .stamp
	mv $A $(FTP)

diff:	clean
	tar zxf $(FTP)/$A
	diff $D .

# eof
