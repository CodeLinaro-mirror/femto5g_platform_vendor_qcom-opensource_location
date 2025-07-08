ifeq ($(TARGET_BOARD_PLATFORM), monaco)
LOCAL_PATH := $(call my-dir)

include $(call all-makefiles-under,$(LOCAL_PATH))
endif
