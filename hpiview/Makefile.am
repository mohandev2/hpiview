## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I /usr/local/include/openhpi \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = hpiview

hpiview_SOURCES = \
	hpiview.c hpiview.h \
	hview_utils.c hview_utils.h \
	hview_service.c hview_service.h \
	hview_widgets.c hview_widgets.h \
	hview_windows.c hview_windows.h \
	hview_callbacks.c hview_callbacks.h \
	voh_types.h voh_types.c \
	voh.c voh.h \
	voh_string.c voh_string.h

hpiview_LDADD = @PACKAGE_LIBS@ -L /usr/local/lib -lopenhpi

