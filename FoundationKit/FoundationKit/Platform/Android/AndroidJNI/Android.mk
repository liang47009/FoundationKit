
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := androidjni
LOCAL_MODULE_FILENAME := libandroidjni

LOCAL_SRC_FILES := ./AndroidJNI.cpp

LOCAL_EXPORT_C_INCLUDES := ./../

LOCAL_EXPORT_CPPFLAGS := -frtti -fexceptions -fsigned-char -std=c++11

LOCAL_EXPORT_LDLIBS := -llog \
                       -landroid

include $(BUILD_STATIC_LIBRARY)




