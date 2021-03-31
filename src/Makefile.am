AM_CPPFLAGS = -D_GNU_SOURCE
AM_CFLAGS = -std=gnu99 -Wall -pedantic -Werror=implicit $(GTK_CFLAGS)

bin_PROGRAMS = simpledevelop

BUILT_SOURCES = resources.c resources.h

simpledevelop_SOURCES = 	\
	main.c			\
	resources.c		\
	resources.h		\
	sd-application.c	\
	sd-application.h	\
	sd-window.c	\
	sd-window.h

simpledevelop_LDADD = $(GTK_LIBS)

resources = org.xnsc.simpledevelop.gresource.xml
resources.c: $(resources)
	$(AM_V_GEN) glib-compile-resources --sourcedir=$(srcdir) --target=$@ \
	    --generate-source --c-name=simpledevelop $(srcdir)/$(resources)
resources.h: $(resources)
	$(AM_V_GEN) glib-compile-resources --sourcedir=$(srcdir) --target=$@ \
	    --generate-header --c-name=simpledevelop $(srcdir)/$(resources)

appdir = $(datadir)/applications
app_DATA = simpledevelop.desktop

CLEANFILES = *~

EXTRA_DIST =		\
	window.glade	\
	$(resources)
