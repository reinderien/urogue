#!/usr/bin/env -S make -j -f

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

flags=-Wall -std=c11
ifeq ($(PROFILE),DEBUG)
	flags += -ggdb
else
	# Oz is like Os but harder
	flags += -Oz -march=native
endif

cflags=$(flags) $(shell $(pkgenv) --cflags)

ldflags=$(flags) $(shell $(pkgenv) --libs)
ifeq ($(PROFILE),DEBUG)
	# Nothing else here
else
	ldflags += -Wl,-no_pie -Wl,-S -Wl,-x -Wl,-dead_strip_dylibs
endif

mods=error main mobs util view wave


.PHONY: all clean

all: urogue

urogue: $(mods:=.o)
	gcc -o $@ $^ $$ldflags

%.o: %.d
	gcc -o $@ $*.c $$cflags -c

%.d: %.c makefile
	gcc -o $@ $< $$cflags -M

-include $(mods:=.d)

clean:
	rm -f *.o *.d urogue
