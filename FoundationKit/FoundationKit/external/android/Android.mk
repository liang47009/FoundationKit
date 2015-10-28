LOCAL_PATH := $(call my-dir)

#config for curl
include $(CLEAR_VARS)
LOCAL_MODULE := libcurl_static
LOCAL_MODULE_FILENAME := curl
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libs/libcurl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/$(TARGET_ARCH_ABI)/include/curl
LOCAL_STATIC_LIBRARIES += libssl_static
LOCAL_STATIC_LIBRARIES += libcrypto_static
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto_static
LOCAL_MODULE_FILENAME := crypto
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libs/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE := libssl_static
LOCAL_MODULE_FILENAME := ssl
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libs/libssl.a
include $(PREBUILT_STATIC_LIBRARY)

#minizip
include $(CLEAR_VARS)
LOCAL_MODULE := libminizip_static
LOCAL_MODULE_FILENAME := minizip
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libs/libminizip.a
include $(PREBUILT_STATIC_LIBRARY)

##zlib
include $(CLEAR_VARS)
LOCAL_MODULE := libzlib_static
LOCAL_MODULE_FILENAME := zlib
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libs/libz.a
include $(PREBUILT_STATIC_LIBRARY)

##luajit
# jit only when not using mips
# non-jit for mips
include $(CLEAR_VARS)
LOCAL_MODULE := libluajit_static
LOCAL_MODULE_FILENAME := luajit
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libs/libluajit.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/$(TARGET_ARCH_ABI)/include/luajit
include $(PREBUILT_STATIC_LIBRARY)

