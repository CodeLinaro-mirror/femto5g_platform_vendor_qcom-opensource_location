AM_CFLAGS = \
     -I./ \
     -D__STDC_FORMAT_MACROS \
     $(GPSUTILS_CFLAGS) \
     -std=c++17

if USE_G_STRLCPY
    AM_CFLAGS += -Dstrlcpy=g_strlcpy
endif
if USE_G_STRLCAT
    AM_CFLAGS += -Dstrlcat=g_strlcat
endif

ACLOCAL_AMFLAGS = -I m4

requiredlibs = \
    $(GPSUTILS_LIBS) \
    -lprotobuf-lite
if PROTOBUF_INC_LIBS
requiredlibs += \
    -labsl_strings \
    -labsl_log_internal_message \
    -labsl_log_internal_proto \
    -labsl_log_internal_check_op
endif

liblocation_api_msg_proto_la_SOURCES = \
    LocationApiDataTypes.pb.cc \
    LocationApiMsg.pb.cc \
    LocationApiMsg.cpp \
    LocationApiPbMsgConv.cpp

library_include_HEADERS = \
    LocationApiMsg.h \
    LocationApiDataTypes.pb.h \
    LocationApiMsg.pb.h \
    LocationApiPbMsgConv.h

if USE_GLIB
liblocation_api_msg_proto_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@ -include glib.h
liblocation_api_msg_proto_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
liblocation_api_msg_proto_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@ -include glib.h
else
liblocation_api_msg_proto_la_CFLAGS = $(AM_CFLAGS)
liblocation_api_msg_proto_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
liblocation_api_msg_proto_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

liblocation_api_msg_proto_la_LIBADD = $(requiredlibs) -ldl -lstdc++

library_includedir = $(pkgincludedir)
#Create and Install libraries
lib_LTLIBRARIES = liblocation_api_msg_proto.la
