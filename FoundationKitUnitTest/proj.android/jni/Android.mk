LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := foundationkittest_static
LOCAL_MODULE_FILENAME := libfoundationkittest

LOCAL_SRC_FILES := main.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/../../../FoundationKit \
                    $(LOCAL_PATH)/jemalloc/$(TARGET_ARCH_ABI)/include

LOCAL_WHOLE_STATIC_LIBRARIES += foundationkit_static
LOCAL_STATIC_LIBRARIES := foundationkittest_static

include $(BUILD_SHARED_LIBRARY)
include $(LOCAL_PATH)/../../../FoundationKit/proj.android/jni/Android.mk

