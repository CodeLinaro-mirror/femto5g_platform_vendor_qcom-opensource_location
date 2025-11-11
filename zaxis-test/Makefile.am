AM_CFLAGS = \
    -DDEBUG \
    $(GPSUTILS_CFLAGS) \
    $(LOCMQCLIENT_CFLAGS) \
    $(IZATCLIENTAPI_CFLAGS) \
    $(LOCPLA_CFLAGS) \
    $(NLPAPI_CFLAGS) \
    -DUSE_ANDROID_LOGGING \
    -std=c++14

AM_CPPFLAGS = $(AM_CFLAGS)

## Required libraries
required_libs = \
    $(GPSUTILS_LIBS) \
    $(CUTILS_LIBS) \
    $(IZATCLIENTAPI_LIBS) \
    $(LOCMQCLIENT_LIBS) \
    $(LOCBASEUTIL_LIBS) \
    -ldl


## zaxis_test C sources
zaxis_test_c_sources = \
    ZAxisTest.cpp

######################
# Build zaxis_test
######################
zaxis_test_SOURCES = $(zaxis_test_c_sources)

if USE_GLIB
zaxis_test_CFLAGS = -g -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
zaxis_test_LDFLAGS = -lstdc++ -lpthread @GLIB_LIBS@ -shared -avoid-version
zaxis_test_CPPFLAGS = -g -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
zaxis_test_CFLAGS = $(AM_CFLAGS)
zaxis_test_LDFLAGS = -lpthread -shared -version-info 1:0:0
zaxis_test_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

zaxis_test_LDADD = $(required_libs)

bin_PROGRAMS = zaxis_test
