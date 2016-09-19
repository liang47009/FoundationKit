# (default)	The default minimal system C++ runtime library.	N/A
#APP_STL :=libstdc++
#	The GAbi++ runtime (static).	C++ Exceptions and RTTI
#APP_STL :=gabi++_static
#	The GAbi++ runtime (shared).	C++ Exceptions and RTTI
#APP_STL :=gabi++_shared
#	The STLport runtime (static).	C++ Exceptions and RTTI; Standard Library
#APP_STL :=stlport_static
#The STLport runtime (shared).	C++ Exceptions and RTTI; Standard Library
#APP_STL :=stlport_shared
#	The GNU STL (static).	C++ Exceptions and RTTI; Standard Library
APP_STL :=gnustl_static
#The GNU STL (shared).	C++ Exceptions and RTTI; Standard Library
#APP_STL :=gnustl_shared
#The LLVM libc++ runtime (static).	C++ Exceptions and RTTI; Standard Library
#APP_STL :=c++_static
#The LLVM libc++ runtime (shared).	C++ Exceptions and RTTI; Standard Library
#APP_STL :=c++_shared

#APP_ABI := all
#APP_ABI := armeabi armeabi-v7a arm64-v8a x86 x86_64 mips mips64
APP_ABI := armeabi armeabi-v7a x86

#NDK_TOOLCHAIN_VERSION := 4.8
NDK_TOOLCHAIN_VERSION := clang

#APP_PLATFORM=android-19

APP_CPPFLAGS := -frtti -fexceptions -fsigned-char -std=c++11
APP_LDFLAGS := -latomic -llog -landroid

APP_DEBUG := $(strip $(NDK_DEBUG))
ifeq ($(APP_DEBUG),1)
  APP_CPPFLAGS += -DDEBUG
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif













