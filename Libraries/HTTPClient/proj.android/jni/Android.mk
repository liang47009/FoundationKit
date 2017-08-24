
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
SRCROOT := ../..
PROJECT_DIR=$(LOCAL_PATH)/../..

LOCAL_MODULE := httpclient
LOCAL_MODULE_FILENAME := libhttpclient
LOCAL_SRC_FILES := \
$(SRCROOT)/HTTPClient/HTTPClient.cpp \
$(SRCROOT)/HTTPClient/HTTPCode.cpp \
$(SRCROOT)/HTTPClient/HTTPDownloader.cpp \
$(SRCROOT)/HTTPClient/HTTPRequest.cpp \
$(SRCROOT)/HTTPClient/HTTPResponse.cpp \
$(SRCROOT)/HTTPClient/HTTPUploader.cpp \
$(SRCROOT)/HTTPClient/libcurl_init.cpp \
$(SRCROOT)/HTTPClient/MimeTypes.cpp \

LOCAL_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/../../ThirdParty/curl/include/android/curl/ \

LOCAL_EXPORT_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/../../ThirdParty/curl/include/android/curl/ \

LOCAL_SHORT_COMMANDS := true

TARGET_LOCAL_CFLAGS := \
-fexceptions

TARGET_LOCAL_CPPFLAGS := \
-frtti \
-fexceptions \
-fsigned-char \
-std=c++11 \
-Wno-deprecated-declarations \
-DUSE_FILE32API \
-DANDROID

TARGET_LOCAL_LDLIBS := \
-lz

ifeq ($(NDK_DEBUG),1)
  TARGET_LOCAL_CFLAGS += -DDEBUG
  TARGET_LOCAL_CPPFLAGS += -DDEBUG
else
  TARGET_LOCAL_CFLAGS += -DNDEBUG
  TARGET_LOCAL_CPPFLAGS += -DNDEBUG
endif

ifeq (armeabi-v7a,$(TARGET_ARCH_ABI))
TARGET_LOCAL_CPPFLAGS += -DUSE_NEON
endif

LOCAL_CFLAGS := $(TARGET_LOCAL_CFLAG)
LOCAL_CPPFLAGS := $(TARGET_LOCAL_CPPFLAGS)
LOCAL_LDLIBS     := $(TARGET_LOCAL_LDLIBS)
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_EXPORT_CPPFLAGS := $(LOCAL_CPPFLAGS)
LOCAL_EXPORT_LDLIBS := $(LOCAL_EXPORT_LDLIBS)

#Reduce Binaries Size
LOCAL_CPPFLAGS += -ffunction-sections -fdata-sections -fvisibility=hidden
LOCAL_CFLAGS   += -ffunction-sections -fdata-sections -fvisibility=hidden 
LOCAL_LDFLAGS  += -Wl,--gc-sections

LOCAL_STATIC_LIBRARIES += cocos_crypto_static cocos_ssl_static cocos_curl_static

LOCAL_SHARED_LIBRARIES += foundationkit

include $(BUILD_SHARED_LIBRARY)
$(call import-add-path,$(PROJECT_DIR)/../../)
$(call import-module,FoundationKit/proj.android/jni)
$(call import-module,ThirdParty/openssl/prebuilt/android)
$(call import-module,ThirdParty/curl/prebuilt/android)












