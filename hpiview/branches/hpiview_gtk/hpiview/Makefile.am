## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I /usr/local/include/openhpi \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = hpiview

hpiview_SOURCES = \
	hpiview.c hpiview.h \
	voh.c voh.h \
	voh_string.c voh_string.h

hpiview_LDADD = @PACKAGE_LIBS@ -L /usr/local/lib -lopenhpi

