AM_CFLAGS = \
    -I. \
    $(LOCPLA_CFLAGS) \
    $(GPSUTILS_CFLAGS) \
    $(LOC_CORE_CFLAGS) \
    $(LOC_SLL_IF_CFLAGS) \
    -fno-short-enums \
    -D__func__=__PRETTY_FUNCTION__ \
    -DTARGET_USES_QCOM_BSP

libsynergy_loc_api_la_SOURCES = \
    SynergyLocApi.cpp

if USE_GLIB
libsynergy_loc_api_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libsynergy_loc_api_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -lcutils @GLIB_LIBS@ -shared -version-info 1:0:0
libsynergy_loc_api_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libsynergy_loc_api_la_CFLAGS = $(AM_CFLAGS)
libsynergy_loc_api_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -lcutils -shared -version-info 1:0:0
libsynergy_loc_api_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libsynergy_loc_api_la_CXXFLAGS = -std=c++0x
libsynergy_loc_api_la_LIBADD = -lstdc++ -ldl -lutils $(GPSUTILS_LIBS) $(LOC_CORE_LIBS)

library_include_HEADERS = \
    SynergyLocApi.h \
    synergy_loc_util_log.h

library_includedir = $(pkgincludedir)/synergy_loc_api

#Create and Install libraries
lib_LTLIBRARIES = libsynergy_loc_api.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = synergy-loc-api.pc
EXTRA_DIST = $(pkgconfig_DATA)
