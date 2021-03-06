LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := foundationkittest_static
LOCAL_MODULE_FILENAME := libfoundationkittest

LOCAL_SRC_FILES := main.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/../../../FoundationKit

LOCAL_LDLIBS := -latomic -landroid -llog -lz

#Address Sanitizer
# LOCAL_CPPFLAGS += -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-call -fPIE -pie -g -O2
# LOCAL_CFLAGS   += -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-call -fPIE -pie -g -O2
# LOCAL_LDFLAGS  += -fsanitize=address -O1
# LOCAL_ARM_MODE += arm

#DrMemory
# LOCAL_CPPFLAGS += -g -fno-omit-frame-pointer -O0
# LOCAL_CFLAGS   += -g -fno-omit-frame-pointer -O0

#LOCAL_STATIC_LIBRARIES += foundationkit
LOCAL_WHOLE_STATIC_LIBRARIES+= foundationkit
include $(BUILD_SHARED_LIBRARY)
$(call import-add-path,$(LOCAL_PATH)/../../..)
$(call import-module,FoundationKit/proj.android/jni)


