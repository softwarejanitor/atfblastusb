#
# Makefile for atfblastusb.  Pretty lame, needs to be improved.
#
NAME=atfblastusb
CFLAGS=-g -Wall -o $(NAME)
#GTKFLAGS=-export-dynamic `pkg-config --cflags --libs gtk+-2.0`
GTKFLAGS=`pkg-config --cflags --libs gtk+-2.0`
SRCS=atfblastusb.c
OBJS=$(SRCS:.c=.o)
HEADERS=atfblastusb.h
CC=gcc
DESTDIR=/usr/local
SHAREDIR=/usr/share

INCLUDES=-I/usr/include/glib-2.0/ \
	-I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ \
	-I/usr/include/pango-1.0/ -I/usr/include/cairo/ \
	-I/usr/include/gdk-pixbuf-2.0/ -I/usr/include/atk-1.0/ \
	-I/usr/include/gtk-unix-print-2.0/

all: atfblastusb

atfblastusb: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) $(GTKFLAGS) $(INCLUDES)

install:
	/usr/bin/install $(NAME) $(DESTDIR)/bin
	/usr/bin/install images/$(NAME).xpm $(SHAREDIR)/icons
	/usr/bin/install images/$(NAME).png $(SHAREDIR)/icons
	/usr/bin/install $(NAME).desktop $(SHAREDIR)/applications

clean:
	/bin/rm -f $(NAME)

