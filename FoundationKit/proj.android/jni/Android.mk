
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
SRCROOT := ../..
PROJECT_DIR:=$(LOCAL_PATH)/../..
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
$(SRCROOT)/FoundationKit/Foundation/ThreadTimer.cpp \
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
$(SRCROOT)/FoundationKit/Platform/Directory.cpp \
$(SRCROOT)/FoundationKit/Platform/File.cpp \
$(SRCROOT)/FoundationKit/Platform/Path.cpp \
$(SRCROOT)/FoundationKit/Platform/Platform.cpp \

LOCAL_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/FoundationKit/external/unzip \
$(PROJECT_DIR)/../ThirdParty/spdlog/include

LOCAL_CFLAGS := \
-fsigned-char \
-DANDROID \
-DNDEBUG \
-DUSE_FILE32API

LOCAL_CPPFLAGS := \
-fsigned-char \
-std=c++14 \
-DANDROID \
-DNDEBUG \
-DUSE_FILE32API

#-ffunction-sections 和 -fdata-sections 将每个函数或符号创建为一个sections，
#其中每个sections名与function或data名保持一致。而在链接阶段， -Wl,–gc-sections 
#指示链接器去掉不用的section（其中-wl, 表示后面的参数 -gc-sections 传递给链接器），
#这样就能减少最终的可执行程序的大小了。
LOCAL_CFLAGS += -ffunction-sections -fdata-sections
LOCAL_CPPFLAGS += -ffunction-sections -fdata-sections

LOCAL_CPPFLAGS += -Os

#编译警告
LOCAL_CPPFLAGS += -Wall

LOCAL_SHORT_COMMANDS := true
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_EXPORT_CPPFLAGS := $(LOCAL_CPPFLAGS)
LOCAL_EXPORT_LDLIBS := -latomic -landroid -llog -lz -lGLESv2
LOCAL_EXPORT_C_INCLUDES :=$(LOCAL_C_INCLUDES)
LOCAL_STATIC_LIBRARIES := cpufeatures
include $(BUILD_STATIC_LIBRARY)
#==============================================================
$(call import-add-path,$(LOCAL_PATH)/../../FoundationKit)
$(call import-module,android/cpufeatures)

$(info ----------------- Compile libfoundationkit infomation -------------------)
$(info TARGET_PLATFORM = $(TARGET_PLATFORM))
$(info TARGET_ARCH     = $(TARGET_ARCH))
$(info TARGET_ARCH_ABI = $(TARGET_ARCH_ABI))
$(info TARGET_ABI      = $(TARGET_ABI))
$(info APP_ABI         = $(APP_ABI))
$(info --------------------------------------------------------------------------)











