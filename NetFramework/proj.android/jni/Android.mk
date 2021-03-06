
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
SRCROOT := ../..
PROJECT_DIR:=$(LOCAL_PATH)/../..
LOCAL_MODULE := netframework
LOCAL_MODULE_FILENAME := libnetframework
LOCAL_SRC_FILES := \
$(SRCROOT)/NetFramework/HTTPClient/HTTPClient.cpp \
$(SRCROOT)/NetFramework/HTTPClient/HTTPCode.cpp \
$(SRCROOT)/NetFramework/HTTPClient/HTTPDownloader.cpp \
$(SRCROOT)/NetFramework/HTTPClient/HTTPRequest.cpp \
$(SRCROOT)/NetFramework/HTTPClient/HTTPResponse.cpp \
$(SRCROOT)/NetFramework/HTTPClient/HTTPUploader.cpp \
$(SRCROOT)/NetFramework/HTTPClient/libcurl_init.cpp 

LOCAL_C_INCLUDES := \
$(PROJECT_DIR)/ \
$(PROJECT_DIR)/NetFramework \
$(PROJECT_DIR)/../FoundationKit/ \
$(PROJECT_DIR)/../ThirdParty/curl/include/android/curl

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
LOCAL_EXPORT_LDLIBS := -latomic
LOCAL_EXPORT_C_INCLUDES :=$(LOCAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)
#==============================================================
$(call import-add-path,$(LOCAL_PATH)/../../NetFramework)

$(info ----------------- Compile libnetframework infomation -------------------)
$(info TARGET_PLATFORM = $(TARGET_PLATFORM))
$(info TARGET_ARCH     = $(TARGET_ARCH))
$(info TARGET_ARCH_ABI = $(TARGET_ARCH_ABI))
$(info TARGET_ABI      = $(TARGET_ABI))
$(info APP_ABI         = $(APP_ABI))
$(info --------------------------------------------------------------------------)











