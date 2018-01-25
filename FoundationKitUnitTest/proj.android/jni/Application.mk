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

#-pipe是个安全而有趣的标记。它对代码生成毫无影响，但是可以加快编译过程。
#此标记指示编译器在不同编译时期使用pipe而不是临时文件。
APP_CPPFLAGS += -pipe
APP_CFLAGS += -pipe

APP_CPPFLAGS += -fstack-protector
APP_CFLAGS += -fstack-protector


# APP_CPPFLAGS += -fno-omit-frame-pointer -fno-optimize-sibling-call -fPIE -pie -g -O1
# APP_CFLAGS   += -fno-omit-frame-pointer -fno-optimize-sibling-call -fPIE -pie -g -O1
APP_LDFLAGS  += -O2
#APP_ARM_MODE += arm

#Address Sanitizer
# APP_CPPFLAGS += -fsanitize=address
# APP_CFLAGS   += -fsanitize=address
# APP_LDFLAGS  += -fsanitize=address

#APP_OPTIM := debug
#APP_CPPFLAGS += -DDEBUG


# APP_CPPFLAGS += -DNDEBUG
# APP_OPTIM := release

