LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../FoundationKit)

#ifeq (armeabi-v7a,$(TARGET_ARCH_ABI))
#LOCAL_ARM_NEON := true
#endif

LOCAL_MODULE := foundationkit
LOCAL_MODULE_FILENAME := libfoundationkit

LOCAL_SRC_FILES := \
../../FoundationKit/Base/Data.cpp \
../../FoundationKit/Base/DataStream.cpp \
../../FoundationKit/Base/MathEx.cpp \
../../FoundationKit/Base/MathUtil.cpp \
../../FoundationKit/Base/Matrix.cpp \
../../FoundationKit/Base/Quaternion.cpp \
../../FoundationKit/Base/Rect.cpp \
../../FoundationKit/Base/Size.cpp \
../../FoundationKit/Base/Value.cpp \
../../FoundationKit/Base/Vector2.cpp \
../../FoundationKit/Base/Vector3.cpp \
../../FoundationKit/Base/Vector4.cpp \
../../FoundationKit/Crypto/aes.cpp \
../../FoundationKit/Crypto/Base64.cpp \
../../FoundationKit/Events/Event.cpp \
../../FoundationKit/Events/EventAcceleration.cpp \
../../FoundationKit/Events/EventController.cpp \
../../FoundationKit/Events/EventCustom.cpp \
../../FoundationKit/Events/EventDispatcher.cpp \
../../FoundationKit/Events/EventKeyboard.cpp \
../../FoundationKit/Events/EventListener.cpp \
../../FoundationKit/Events/EventListenerAcceleration.cpp \
../../FoundationKit/Events/EventListenerController.cpp \
../../FoundationKit/Events/EventListenerCustom.cpp \
../../FoundationKit/Events/EventListenerKeyboard.cpp \
../../FoundationKit/Events/EventListenerMouse.cpp \
../../FoundationKit/Events/EventListenerTouch.cpp \
../../FoundationKit/Events/EventMouse.cpp \
../../FoundationKit/Events/EventTouch.cpp \
../../FoundationKit/Events/GameController.cpp \
../../FoundationKit/Events/GameController-android.cpp \
../../FoundationKit/Events/IMEDispatcher.cpp \
../../FoundationKit/Events/Touch.cpp \
../../FoundationKit/external/ConvertUTF/ConvertUTF.c \
../../FoundationKit/external/ConvertUTF/ConvertUTFWrapper.cpp \
../../FoundationKit/external/unzip/ioapi.cpp \
../../FoundationKit/external/unzip/ioapi_mem.cpp \
../../FoundationKit/external/unzip/unzip.cpp \
../../FoundationKit/Foundation/DelegateManager.cpp \
../../FoundationKit/Foundation/Exception.cpp \
../../FoundationKit/Foundation/Logger.cpp \
../../FoundationKit/Foundation/MessageQueue.cpp \
../../FoundationKit/Foundation/Scheduler.cpp \
../../FoundationKit/Foundation/StringUtils.cpp \
../../FoundationKit/Foundation/ThreadPool.cpp \
../../FoundationKit/Foundation/Version.cpp \
../../FoundationKit/Foundation/WorkQueue.cpp \
../../FoundationKit/HttpClient/HttpClient.cpp \
../../FoundationKit/HttpClient/HttpRequest.cpp \
../../FoundationKit/HttpClient/MimeTypes.cpp \
../../FoundationKit/HttpDownloader/Downloader.cpp \
../../FoundationKit/HttpDownloader/DownloaderImpl.cpp \
../../FoundationKit/LuaEngine/LuaEngine.cpp \
../../FoundationKit/LuaEngine/LuaLoader.cpp \
../../FoundationKit/LuaEngine/LuaStack.cpp \
../../FoundationKit/LuaEngine/LuaValue.cpp \
../../FoundationKit/LuaEngine/Script.cpp \
../../FoundationKit/Platform/Android/AndroidJavaBridge.cpp \
../../FoundationKit/Platform/Android/AndroidSystemInfo.cpp \
../../FoundationKit/Platform/Android/FileUtils_android.cpp \
../../FoundationKit/Platform/Android/PlatformAndroid.cpp \
../../FoundationKit/Platform/Android/ProtectedMemoryAllocator.cpp \
../../FoundationKit/Platform/FileUtils.cpp \
../../FoundationKit/Platform/Platform.cpp

                   

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../ \
                    $(LOCAL_PATH)/../../FoundationKit/external/unzip \
                    $(LOCAL_PATH)/../../FoundationKit/external/android/$(TARGET_ARCH_ABI)/include/curl\
                    $(LOCAL_PATH)/../../FoundationKit/external/android/$(TARGET_ARCH_ABI)/include/luajit

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../FoundationKit/external/unzip \

LOCAL_EXPORT_CPPFLAGS := -frtti -fexceptions -fsigned-char -std=c++11

LOCAL_EXPORT_LDLIBS := -llog \
                       -lz \
                       -landroid

LOCAL_WHOLE_STATIC_LIBRARIES += androidjni
LOCAL_WHOLE_STATIC_LIBRARIES += libluajit_static
LOCAL_WHOLE_STATIC_LIBRARIES += libcurl_static


include $(BUILD_STATIC_LIBRARY)
#==============================================================
$(call import-module,Platform/Android/AndroidJNI)
$(call import-module,external/android)


$(info ----------------- Compile libfoundationkit infomation -------------------)
$(info TARGET_ARCH     = $(TARGET_ARCH))
$(info TARGET_ARCH_ABI = $(TARGET_ARCH_ABI))
$(info TARGET_ABI      = $(TARGET_ABI))
$(info APP_ABI         = $(APP_ABI))
$(info --------------------------------------------------------------------------)

