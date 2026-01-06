LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(TARGET_KAIOS),true)
GNSS_CFLAGS += -DFEATURE_KAIOS
endif

LOCAL_CFLAGS += $(GNSS_CFLAGS)
LOCAL_MODULE := libnlp_api_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/inc

include $(BUILD_HEADER_LIBRARY)

include $(CLEAR_VARS)
