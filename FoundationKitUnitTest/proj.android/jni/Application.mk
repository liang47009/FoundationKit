#设置stl库
APP_STL := gnustl_static
#APP_STL := c++_static
#APP_STL  := stlport_static
#APP_STL  :=libc++_static

#设置编译器和编译器版本
NDK_TOOLCHAIN_VERSION := 4.9
#NDK_TOOLCHAIN_VERSION := clang

#设置编译架构
#armeabi can not support atomic
#APP_ABI := all
#APP_ABI := armeabi armeabi-v7a x86
APP_ABI := armeabi

APP_PLATFORM=android-15

#设置编译选项
APP_CPPFLAGS := -frtti -fexceptions -fsigned-char -std=c++14
APP_LDFLAGS := -latomic -llog -landroid


# APP_CPPFLAGS += -fno-omit-frame-pointer -fno-optimize-sibling-call -fPIE -pie -g -O1
# APP_CFLAGS   += -fno-omit-frame-pointer -fno-optimize-sibling-call -fPIE -pie -g -O1
APP_LDFLAGS  += -O1
APP_ARM_MODE += arm

#Address Sanitizer
# APP_CPPFLAGS += -fsanitize=address
# APP_CFLAGS   += -fsanitize=address
# APP_LDFLAGS  += -fsanitize=address

APP_OPTIM := debug
APP_CPPFLAGS += -DDEBUG


# APP_CPPFLAGS += -DNDEBUG
# APP_OPTIM := release

