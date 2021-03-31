AM_CPPFLAGS = -D_GNU_SOURCE
AM_CFLAGS = -std=gnu99 -Wall -pedantic -Werror=implicit $(GTK_CFLAGS)

bin_PROGRAMS = simpledevelop

simpledevelop_SOURCES = 	\
	main.c			\
	sd-application.c	\
	sd-application.h	\
	sd-application-window.c	\
	sd-application-window.h

simpledevelop_LDADD = $(GTK_LIBS)

CLEANFILES = *~
