
LOCAL_PATH := $(call my-dir)

# include $(CLEAR_VARS)
# LOCAL_MODULE    := libfoundationkit
# LOCAL_SRC_FILES := ../../../../../../ThirdParty/android/libs/$(TARGET_ARCH_ABI)/libfoundationkit.so
# include $(PREBUILT_SHARED_LIBRARY)

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
$(SRCROOT)/FoundationKit/Foundation/Math.cpp \
$(SRCROOT)/FoundationKit/Foundation/MathUtil.cpp \
$(SRCROOT)/FoundationKit/Foundation/Matrix.cpp \
$(SRCROOT)/FoundationKit/Foundation/Quaternion.cpp \
$(SRCROOT)/FoundationKit/Foundation/Rect.cpp \
$(SRCROOT)/FoundationKit/Foundation/StringUtils.cpp \
$(SRCROOT)/FoundationKit/Foundation/Time.cpp \
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
$(SRCROOT)/FoundationKit/Platform/Android/AndroidDirectory.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidEnvironment.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidFile.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidPath.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidPlatform.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidDevice.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidProtectedMemoryAllocator.cpp \
$(SRCROOT)/FoundationKit/Platform/Directory.cpp \
$(SRCROOT)/FoundationKit/Platform/File.cpp \
$(SRCROOT)/FoundationKit/Platform/Path.cpp \
$(SRCROOT)/FoundationKit/Platform/Platform.cpp \

LOCAL_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/FoundationKit/external/unzip \
$(PROJECT_DIR)/../ThirdParty/spdlog/include \
cpufeatures/

#LOCAL_EXPORT_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/FoundationKit/external/unzip \
$(PROJECT_DIR)/../ThirdParty/spdlog/include \
cpufeatures/

TARGET_LOCAL_CFLAGS := \
-fexceptions

TARGET_LOCAL_CPPFLAGS := \
-frtti \
-fexceptions \
-fsigned-char \
-std=c++14 \
-Wno-deprecated-declarations \
-DUSE_FILE32API \
-DANDROID

TARGET_LOCAL_LDFLAGS := -Wl,--gc-sections

TARGET_LOCAL_LDLIBS := \
-latomic \
-landroid \
-llog \
-lz \
-lGLESv2

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

#Reduce Binaries Size
# TARGET_LOCAL_CPPFLAGS += -ffunction-sections -fdata-sections -fvisibility=hidden
# TARGET_LOCAL_CFLAGS   += -ffunction-sections -fdata-sections -fvisibility=hidden 
TARGET_LOCAL_CPPFLAGS += -ffunction-sections -fdata-sections
TARGET_LOCAL_CFLAGS   += -ffunction-sections -fdata-sections


LOCAL_SHORT_COMMANDS := true

LOCAL_CFLAGS   := $(TARGET_LOCAL_CFLAG)
LOCAL_CPPFLAGS := $(TARGET_LOCAL_CPPFLAGS)
LOCAL_LDFLAGS  := $(TARGET_LOCAL_LDFLAGS)
LOCAL_LDLIBS   := $(TARGET_LOCAL_LDLIBS)

#LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
#LOCAL_EXPORT_CPPFLAGS := $(LOCAL_CPPFLAGS)
#LOCAL_EXPORT_LDLIBS := $(LOCAL_EXPORT_LDLIBS)

#Add cpufeatures library
LOCAL_STATIC_LIBRARIES += cpufeatures
include $(BUILD_SHARED_LIBRARY)
#LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
#include $(BUILD_STATIC_LIBRARY)
$(call import-add-path,$(PROJECT_DIR)/FoundationKit)
$(call import-module,android/cpufeatures)

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









