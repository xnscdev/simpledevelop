AM_CPPFLAGS = -D_GNU_SOURCE
AM_CFLAGS = -std=gnu99 -Wall -pedantic -Werror=implicit $(GTK_CFLAGS)

bin_PROGRAMS = simpledevelop

BUILT_SOURCES = resources.c resources.h

simpledevelop_SOURCES = 	\
	callbacks.h		\
	expand-tree.c		\
	main.c			\
	resources.c		\
	resources.h		\
	sd-application.c	\
	sd-application.h	\
	sd-window.c		\
	sd-window.h

simpledevelop_LDADD = $(GTK_LIBS)

resources = org.xnsc.simpledevelop.gresource.xml
resources.c: $(resources) window.glade
	$(AM_V_GEN) glib-compile-resources --sourcedir=$(srcdir) --target=$@ \
	    --generate-source --c-name=simpledevelop $(srcdir)/$(resources)
resources.h: $(resources) window.glade
	$(AM_V_GEN) glib-compile-resources --sourcedir=$(srcdir) --target=$@ \
	    --generate-header --c-name=simpledevelop $(srcdir)/$(resources)

appdir = $(datadir)/applications
app_DATA = simpledevelop.desktop

CLEANFILES = *~ resources.c resources.h

EXTRA_DIST =		\
	window.glade	\
	$(resources)
