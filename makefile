#!/usr/bin/env make -j -f

# Requirements:
#   Ubuntu
#     apt install libncursesw5-dev
#   OSX
#     brew install ncurses

export

# For HomeBrew compatibility
ifeq ($(shell uname),Darwin)
	pkgpath=PKG_CONFIG_PATH=/usr/local/opt/ncurses/lib/pkgconfig
endif

pkgenv=$(pkgpath) pkg-config ncursesw

flags=-Wall -std=c11 -ggdb
cflags=$(flags) $(shell $(pkgenv) --cflags)
ldflags=$(flags) $(shell $(pkgenv) --libs)

.PHONY: all clean

all: urogue

urogue: error.o main.o mobs.o view.o
	gcc -o $@ $^ $$ldflags

%.o: %.d
	gcc -o $@ $*.c $$cflags -c

%.d: %.c makefile
	gcc -o $@ $< $$cflags -M

-include error.d main.d mobs.d view.d

clean:
	rm -f *.o *.d urogue
