LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := foundationkittest_static
LOCAL_MODULE_FILENAME := libfoundationkittest

LOCAL_SRC_FILES := main.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/../../../FoundationKit

LOCAL_WHOLE_STATIC_LIBRARIES += foundationkit

include $(BUILD_SHARED_LIBRARY)
$(call import-add-path,$(LOCAL_PATH)/../../..)

$(call import-module,FoundationKit/proj.android/jni)


