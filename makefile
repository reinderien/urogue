#!/usr/bin/env make -f

pkgenv=PKG_CONFIG_PATH=/usr/local/opt/ncurses/lib/pkgconfig pkg-config

cflags=-Wall -std=c17
ldflags=$(cflags)
cinc=$(shell $(pkgenv) ncursesw --cflags)
ldinc=$(shell $(pkgenv) ncursesw --libs)

all: urogue

urogue: main.o mobs.o
	gcc $(ldflags) -o $@ $^ $(ldinc)

%.o: %.d
	gcc $(cflags) -o $@ $*.c $(cinc) -c

%.d: %.c
	gcc $(cflags) -o $@ $< $(cinc) -M

-include main.d mobs.d
