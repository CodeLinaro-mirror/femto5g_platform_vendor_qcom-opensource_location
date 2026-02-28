# Set required flags
GNSS_CFLAGS := \
    -Werror \
    -Wformat \
    -Wformat-extra-args \
    -Wunused-label \
    -Wunused-variable \
    -Wunused-function \
    -Wimplicit-fallthrough \
    -Wno-unused-parameter \
    -Wno-error=unused-parameter \
    -Wno-error=macro-redefined \
    -Wno-error=reorder \
    -Wno-error=missing-braces \
    -Wno-error=self-assign \
    -Wno-error=enum-conversion \
    -Wno-error=logical-op-parentheses \
    -Wno-error=null-arithmetic \
    -Wno-error=null-conversion \
    -Wno-error=parentheses-equality \
    -Wno-error=undefined-bool-conversion \
    -Wno-error=tautological-compare \
    -Wno-error=switch \
    -D_ANDROID_

LOCAL_PATH := $(call my-dir)

include $(call all-makefiles-under,$(LOCAL_PATH))

