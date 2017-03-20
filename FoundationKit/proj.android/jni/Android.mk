
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := foundationkit
LOCAL_MODULE_FILENAME := libfoundationkit

LOCAL_SRC_FILES := \
../../FoundationKit/Crypto/aes.cpp \
../../FoundationKit/Crypto/Base64.cpp \
../../FoundationKit/external/ConvertUTF/ConvertUTFWrapper.cpp \
../../FoundationKit/external/ConvertUTF/ConvertUTF.c \
../../FoundationKit/external/unzip/ioapi.cpp \
../../FoundationKit/external/unzip/ioapi_mem.cpp \
../../FoundationKit/external/unzip/unzip.cpp \
../../FoundationKit/Foundation/Compression.cpp \
../../FoundationKit/Foundation/DataStream.cpp \
../../FoundationKit/Foundation/DateTime.cpp \
../../FoundationKit/Foundation/DelegateManager.cpp \
../../FoundationKit/Foundation/Exception.cpp \
../../FoundationKit/Foundation/Logger.cpp \
../../FoundationKit/Foundation/Math.cpp \
../../FoundationKit/Foundation/MathUtil.cpp \
../../FoundationKit/Foundation/Matrix.cpp \
../../FoundationKit/Foundation/Quaternion.cpp \
../../FoundationKit/Foundation/Rect.cpp \
../../FoundationKit/Foundation/Size.cpp \
../../FoundationKit/Foundation/StringUtils.cpp \
../../FoundationKit/Foundation/Timer.cpp \
../../FoundationKit/Foundation/TimerQueue.cpp \
../../FoundationKit/Foundation/Timespan.cpp \
../../FoundationKit/Foundation/Vector2.cpp \
../../FoundationKit/Foundation/Vector3.cpp \
../../FoundationKit/Foundation/Vector4.cpp \
../../FoundationKit/Foundation/Version.cpp \
../../FoundationKit/Foundation/Value.cpp \
../../FoundationKit/GenericPlatformMacros.cpp \
../../FoundationKit/Platform/Android/AndroidBridge.cpp \
../../FoundationKit/Platform/Android/AndroidJavaBridge.cpp \
../../FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.cpp \
../../FoundationKit/Platform/Android/EnvironmentAndroid.cpp \
../../FoundationKit/Platform/Android/FileUtils_android.cpp \
../../FoundationKit/Platform/Android/PlatformAndroid.cpp \
../../FoundationKit/Platform/Android/ProtectedMemoryAllocator.cpp \
../../FoundationKit/Platform/FileUtils.cpp \
../../FoundationKit/Platform/Platform.cpp \

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../ \
$(LOCAL_PATH)/../../FoundationKit/external/unzip

LOCAL_EXPORT_C_INCLUDES := \
$(LOCAL_PATH)/../../ \
$(LOCAL_PATH)/../../FoundationKit/external/unzip

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

LOCAL_WHOLE_STATIC_LIBRARIES += androidjni

include $(BUILD_STATIC_LIBRARY)
$(call import-add-path,$(LOCAL_PATH)/../../FoundationKit)
$(call import-module,Platform/Android/AndroidJNI)

#==============================================================
$(info ----------------- Compile libfoundationkit infomation -------------------)
$(info TARGET_ARCH     = $(TARGET_ARCH))
$(info TARGET_ARCH_ABI = $(TARGET_ARCH_ABI))
$(info TARGET_ABI      = $(TARGET_ABI))
$(info APP_ABI         = $(APP_ABI))
$(info NDK_DEBUG       = $(NDK_DEBUG))
$(info --------------------------------------------------------------------------)









