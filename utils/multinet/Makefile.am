AM_CFLAGS = \
    -DDEBUG \
    -I ./ \
    $(GPSUTILS_CFLAGS) \
    -std=c++11

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -std=c++11

requiredlibs = \
    $(GPSUTILS_LIBS)

h_sources = \
    LocMultinet.h

c_sources = \
    LocMultinet.cpp

libloc_multinet_la_SOURCES = \
    $(c_sources) $(h_sources)

######################
# Build loc_multinet
######################

if USE_GLIB
libloc_multinet_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_multinet_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_multinet_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_multinet_la_CFLAGS = $(AM_CFLAGS)
libloc_multinet_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libloc_multinet_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_multinet_la_LIBADD = $(requiredlibs) -lstdc++

#Create and Install libraries
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = libloc_multinet.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-multinet.pc
EXTRA_DIST = $(pkgconfig_DATA)

