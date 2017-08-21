
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

LOCAL_CFLAGS :=  -fexceptions
LOCAL_CPPFLAGS := -frtti -fexceptions -fsigned-char -Wno-deprecated-declarations -std=c++11
LOCAL_CFLAGS += -DUSE_FILE32API -DANDROID
LOCAL_CPPFLAGS +=-DUSE_FILE32API -DANDROID

LOCAL_EXPORT_CFLAGS := -fexceptions
LOCAL_EXPORT_CPPFLAGS := -frtti -fexceptions -fsigned-char -Wno-deprecated-declarations -std=c++11
LOCAL_EXPORT_CFLAGS += -DUSE_FILE32API -DANDROID
LOCAL_EXPORT_CPPFLAGS +=  -DUSE_FILE32API -DANDROID

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

LOCAL_LDLIBS     := -landroid -llog
LOCAL_EXPORT_LDLIBS     := -landroid -llog

LOCAL_STATIC_LIBRARIES += cocos_crypto_static cocos_ssl_static cocos_curl_static

LOCAL_SHARED_LIBRARIES += foundationkit

include $(BUILD_SHARED_LIBRARY)
$(call import-add-path,$(PROJECT_DIR)/../../)
$(call import-module,FoundationKit/proj.android/jni)
$(call import-module,ThirdParty/openssl/prebuilt/android)
$(call import-module,ThirdParty/curl/prebuilt/android)












