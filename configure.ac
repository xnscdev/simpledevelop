AC_INIT([simpledevelop], [1.0.0], [xnscdev@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE

AC_PROG_CC

PKG_CHECK_MODULES([GTK], [gtk+-3.0])
AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

AC_CONFIG_FILES([Makefile src/Makefile src/simpledevelop.desktop])
AC_OUTPUT
