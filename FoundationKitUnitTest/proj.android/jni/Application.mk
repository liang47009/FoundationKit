#设置stl库
#APP_STL := gnustl_static
APP_STL := c++_static
#APP_STL  := stlport_static
#APP_STL  :=libc++_static

#设置编译器和编译器版本
#NDK_TOOLCHAIN_VERSION := 4.9
NDK_TOOLCHAIN_VERSION := clang3.5

#设置编译架构
#armeabi can not support atomic
#APP_ABI := all
#APP_ABI := armeabi armeabi-v7a x86
APP_ABI := armeabi

APP_PLATFORM=android-15

#设置编译选项
APP_CPPFLAGS := -frtti -fexceptions -fsigned-char -std=c++14
APP_LDFLAGS := -latomic -llog -landroid

APP_DEBUG := $(strip $(NDK_DEBUG))
ifeq ($(APP_DEBUG),1)
  APP_CPPFLAGS += -DDEBUG
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
