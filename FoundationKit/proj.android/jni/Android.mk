
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
$(SRCROOT)/FoundationKit/external/ConvertUTF/ConvertUTF.cpp \
$(SRCROOT)/FoundationKit/external/unzip/ioapi.cpp \
$(SRCROOT)/FoundationKit/external/unzip/ioapi_mem.cpp \
$(SRCROOT)/FoundationKit/external/unzip/unzip.cpp \
$(SRCROOT)/FoundationKit/Foundation/BitStream.cpp \
$(SRCROOT)/FoundationKit/Foundation/Compression.cpp \
$(SRCROOT)/FoundationKit/Foundation/DateTime.cpp \
$(SRCROOT)/FoundationKit/Foundation/NotificationCenter.cpp \
$(SRCROOT)/FoundationKit/Foundation/Exception.cpp \
$(SRCROOT)/FoundationKit/Foundation/StringUtils.cpp \
$(SRCROOT)/FoundationKit/Foundation/Time.cpp \
$(SRCROOT)/FoundationKit/Foundation/TimeSpan.cpp \
$(SRCROOT)/FoundationKit/Foundation/TimeZone.cpp \
$(SRCROOT)/FoundationKit/Foundation/Version.cpp \
$(SRCROOT)/FoundationKit/Foundation/Value.cpp \
$(SRCROOT)/FoundationKit/GenericPlatformMacros.cpp \
$(SRCROOT)/FoundationKit/Math/Math.cpp \
$(SRCROOT)/FoundationKit/Math/MathUtil.cpp \
$(SRCROOT)/FoundationKit/Math/Matrix.cpp \
$(SRCROOT)/FoundationKit/Math/Quaternion.cpp \
$(SRCROOT)/FoundationKit/Math/Rect.cpp \
$(SRCROOT)/FoundationKit/Math/Vector2.cpp \
$(SRCROOT)/FoundationKit/Math/Vector3.cpp \
$(SRCROOT)/FoundationKit/Math/Vector4.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidJNI/AndroidJNIBridge.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidDevice.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidFile.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidPath.cpp \
$(SRCROOT)/FoundationKit/Platform/Android/AndroidPlatformMisc.cpp \
$(SRCROOT)/FoundationKit/Platform/Directory.cpp \
$(SRCROOT)/FoundationKit/Platform/File.cpp \
$(SRCROOT)/FoundationKit/Platform/Path.cpp \
$(SRCROOT)/FoundationKit/Platform/Standard/StandardDirectory.cpp \
$(SRCROOT)/FoundationKit/Platform/Standard/StandardEnvironment.cpp \
$(SRCROOT)/FoundationKit/Platform/Standard/StandardFile.cpp 

LOCAL_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/FoundationKit/external/unzip

LOCAL_CFLAGS := \
-fsigned-char \
-DANDROID \
-DUSE_FILE32API

LOCAL_CPPFLAGS := \
-fsigned-char \
-std=c++14 \
-DANDROID \
-DUSE_FILE32API

#优化等级
LOCAL_CPPFLAGS += -O2

#编译警告
LOCAL_CPPFLAGS += -Wall

#-ffunction-sections 和 -fdata-sections 将每个函数或符号创建为一个sections，
#其中每个sections名与function或data名保持一致。而在链接阶段， -Wl,–gc-sections 
#指示链接器去掉不用的section（其中-wl, 表示后面的参数 -gc-sections 传递给链接器），
#这样就能减少最终的可执行程序的大小了。
LOCAL_CFLAGS += -ffunction-sections -fdata-sections
LOCAL_CPPFLAGS += -ffunction-sections -fdata-sections

#这是个用来削减代码尺寸的常用标记。在所有不影响除错（例如x86-64）的构架上，
#所有的-O等级（除了-O0）中都启用了它，但是也可能需要手动添加到你的标记中。
#尽管GNU gcc手册没有明确指出-O会启用这个标记的所有构架，你需要在x86上手动
#启用它。使用这个标记会使除错难以进行。特别的，它会使排查Java程序的故障变
#得困难，尽管Java代码并不是唯一受此选项影响的代码。所以此标记虽然有用，但
#也会使除错变得困难，特别是backtrace将变得毫无用处。然而，你不准备做软件除
#错并且没有在CFLAGS中加入-ggdb之类与除错相关的标记的话，那就可以试试
#-fomit-frame-pointer。 
#重要: 请勿联合使用-fomit-frame-pointer和与之相似的-momit-leaf-frame-pointer。
#开启后者并无多大用处，因为-fomit-frame-pointer已经把事情搞定了。此外，
#-momit-leaf-frame-pointer还将降低代码性能。
LOCAL_CFLAGS += -fomit-frame-pointer
LOCAL_CPPFLAGS += -fomit-frame-pointer

LOCAL_SHORT_COMMANDS := true
LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_EXPORT_CPPFLAGS := $(LOCAL_CPPFLAGS)
LOCAL_EXPORT_LDLIBS := -latomic -landroid -llog -lz
LOCAL_EXPORT_C_INCLUDES :=$(LOCAL_C_INCLUDES)

#LOCAL_STATIC_LIBRARIES := cpufeatures
LOCAL_WHOLE_STATIC_LIBRARIES :=cpufeatures
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











