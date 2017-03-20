LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := httpclient
LOCAL_MODULE_FILENAME := libhttpclient

LOCAL_SRC_FILES := \
../../HTTPClient/libcurl_init.cpp \
../../HTTPClient/HTTPClient.cpp \
../../HTTPClient/HTTPRequest.cpp \
../../HTTPClient/HTTPResponse.cpp \
../../HTTPClient/MimeTypes.cpp

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../ \
$(LOCAL_PATH)/../../../FoundationKit/ \
$(LOCAL_PATH)/../../HTTPClient/external/android/$(TARGET_ARCH_ABI)/include/curl


LOCAL_EXPORT_C_INCLUDES := \
$(LOCAL_PATH)/../../ \
$(LOCAL_PATH)/../../../FoundationKit/ \
$(LOCAL_PATH)/../../HTTPClient/external/android/$(TARGET_ARCH_ABI)/include/curl

LOCAL_CFLAGS :=  -DUSE_FILE32API -DANDROID
LOCAL_EXPORT_CFLAGS += -DUSE_FILE32API -DANDROID

LOCAL_CPPFLAGS += -frtti -fexceptions -fsigned-char -std=c++11 -DUSE_FILE32API -DANDROID
LOCAL_EXPORT_CPPFLAGS   += -frtti -fexceptions -fsigned-char -std=c++11 -DUSE_FILE32API -DANDROID

LOCAL_EXPORT_LDLIBS     += -llog -lz -landroid

ifeq ($(NDK_DEBUG),1)
  LOCAL_CPPFLAGS += -DDEBUG
  LOCAL_EXPORT_CPPFLAGS += -DDEBUG
else
  LOCAL_CPPFLAGS += -DNDEBUG
  LOCAL_EXPORT_CPPFLAGS += -DNDEBUG
endif

ifeq (armeabi-v7a,$(TARGET_ARCH_ABI))
LOCAL_EXPORT_CPPFLAGS += -DUSE_NEON
endif

LOCAL_STATIC_LIBRARIES += libcurl_static

include $(BUILD_STATIC_LIBRARY)
$(call import-add-path,$(LOCAL_PATH)/../../HTTPClient)
$(call import-module,external/android)

#==============================================================
$(info ----------------- Compile libhttpclient infomation -------------------)
$(info TARGET_ARCH     = $(TARGET_ARCH))
$(info TARGET_ARCH_ABI = $(TARGET_ARCH_ABI))
$(info TARGET_ABI      = $(TARGET_ABI))
$(info APP_ABI         = $(APP_ABI))
$(info NDK_DEBUG       = $(NDK_DEBUG))
$(info --------------------------------------------------------------------------)









