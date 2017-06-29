
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
SRCROOT := ../..
PROJECT_DIR=$(LOCAL_PATH)/../..

LOCAL_MODULE := foundationkit
LOCAL_MODULE_FILENAME := libfoundationkit
LOCAL_SRC_FILES := \
$(SRCROOT)/FoundationKit/Crypto/aes.cpp \
$(SRCROOT)/FoundationKit/Crypto/Base64.cpp \
$(SRCROOT)/FoundationKit/external/ConvertUTF/ConvertUTFWrapper.cpp \
$(SRCROOT)/FoundationKit/external/ConvertUTF/ConvertUTF.c \
$(SRCROOT)/FoundationKit/external/unzip/ioapi.cpp \
$(SRCROOT)/FoundationKit/external/unzip/ioapi_mem.cpp \
$(SRCROOT)/FoundationKit/external/unzip/unzip.cpp \
$(SRCROOT)/FoundationKit/Foundation/Compression.cpp \
$(SRCROOT)/FoundationKit/Foundation/DateTime.cpp \
$(SRCROOT)/FoundationKit/Foundation/DelegateManager.cpp \
$(SRCROOT)/FoundationKit/Foundation/Exception.cpp \
$(SRCROOT)/FoundationKit/Foundation/Logger.cpp \
$(SRCROOT)/FoundationKit/Foundation/Math.cpp \
$(SRCROOT)/FoundationKit/Foundation/MathUtil.cpp \
$(SRCROOT)/FoundationKit/Foundation/Matrix.cpp \
$(SRCROOT)/FoundationKit/Foundation/Quaternion.cpp \
$(SRCROOT)/FoundationKit/Foundation/Rect.cpp \
$(SRCROOT)/FoundationKit/Foundation/Size.cpp \
$(SRCROOT)/FoundationKit/Foundation/StringUtils.cpp \
$(SRCROOT)/FoundationKit/Foundation/Timer.cpp \
$(SRCROOT)/FoundationKit/Foundation/TimerQueue.cpp \
$(SRCROOT)/FoundationKit/Foundation/Timespan.cpp \
$(SRCROOT)/FoundationKit/Foundation/Vector2.cpp \
$(SRCROOT)/FoundationKit/Foundation/Vector3.cpp \
$(SRCROOT)/FoundationKit/Foundation/Vector4.cpp \
$(SRCROOT)/FoundationKit/Foundation/Version.cpp \
$(SRCROOT)/FoundationKit/Foundation/Value.cpp \
$(SRCROOT)/FoundationKit/GenericPlatformMacros.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidJavaBridge.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/EnvironmentAndroid.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/FileUtilsAndroid.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/PlatformAndroid.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/PlatformDevice.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/ProtectedMemoryAllocator.cpp \
$(SRCROOT)/FoundationKit/Platform/FileUtils.cpp \
$(SRCROOT)/FoundationKit/Platform/Platform.cpp \


LOCAL_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/FoundationKit/external/unzip \
$(PROJECT_DIR)/../ThirdParty/spdlog/include \
cpufeatures/

LOCAL_EXPORT_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/FoundationKit/external/unzip \
$(PROJECT_DIR)/../ThirdParty/spdlog/include \
cpufeatures/

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

LOCAL_LDLIBS     := -landroid -llog -lz -lEGL -lGLESv1_CM -lGLESv2 -lc
LOCAL_EXPORT_LDLIBS     := -landroid -llog -lz -lEGL -lGLESv1_CM -lGLESv2

LOCAL_STATIC_LIBRARIES += cpufeatures androidjni
#LOCAL_WHOLE_STATIC_LIBRARIES += androidjni

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
$(call import-add-path,$(PROJECT_DIR)/FoundationKit)
$(call import-module,android/cpufeatures)
$(call import-module,Platform/Android/AndroidJNI)


#==============================================================
$(info ----------------- Compile libfoundationkit infomation -------------------)
$(info TARGET_ARCH     = $(TARGET_ARCH))
$(info TARGET_ARCH_ABI = $(TARGET_ARCH_ABI))
$(info TARGET_ABI      = $(TARGET_ABI))
$(info APP_ABI         = $(APP_ABI))
$(info NDK_DEBUG       = $(NDK_DEBUG))
$(info LOCAL_PATH      = $(LOCAL_PATH))
$(info SRCROOT         = $(SRCROOT))
$(info --------------------------------------------------------------------------)









