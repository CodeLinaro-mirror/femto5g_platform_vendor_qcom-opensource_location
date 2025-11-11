#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module - common
#
# GENERAL DESCRIPTION
#   Common location service module makefile
#
#=============================================================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := zaxis_test
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    ZAxisTest.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libizat_client_api \
    libgps.utils \
    libloc_base_util \
    libc++ \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(TOP)/external/libcxx/src
LOCAL_HEADER_LIBRARIES := \
    libutils_headers \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_mq_client_headers \
    izat_remote_api_prop_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    libnlp_api_headers

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_EXECUTABLE)
