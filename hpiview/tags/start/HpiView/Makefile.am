
bin_PROGRAMS = HpiView

HpiView_SOURCES = \
	Data.h \
	Data.cpp \
	HpiView.h \
	HpiView.cpp \
	Main.cpp \
	Resource.h \
	Resource.cpp \
	String.h \
	String.cpp

INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
HpiView_LDADD   =  $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = lo32-app-hpiview.png lo16-app-hpiview.png

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

HpiView_METASOURCES = AUTO

# the library search path. 
HpiView_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(HpiView_METASOURCES)

