#设置stl库
# gnustl_statici is not support <codecvt>
APP_STL := gnustl_static
#APP_STL := libc++_static

#设置编译器和编译器版本
#NDK_TOOLCHAIN_VERSION := 4.8
NDK_TOOLCHAIN_VERSION := clang

#设置编译架构
#APP_ABI := all
#APP_ABI := armeabi armeabi-v7a x86

#设置编译选项
APP_CPPFLAGS := -frtti -fexceptions -fsigned-char -std=c++11
APP_LDFLAGS := -latomic -llog

ifeq ($(NDK_DEBUG),1)
  APP_CPPFLAGS += -DDEBUG=1
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
