#!/usr/bin/env make -f

export

# For HomeBrew compatibility
pkgenv=PKG_CONFIG_PATH=/usr/local/opt/ncurses/lib/pkgconfig pkg-config ncursesw

flags=-Wall -std=c17 -ggdb
cflags=$(flags) $(shell $(pkgenv) --cflags)
ldflags=$(flags) $(shell $(pkgenv) --libs)

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
