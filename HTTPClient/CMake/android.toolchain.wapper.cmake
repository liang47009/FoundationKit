#从环境变量中获取NDK的路径，如果获取失败，则退出CMake
if(NOT $ENV{ANDROID_NDK} STREQUAL " " )
	set(ANDROID_NDK $ENV{ANDROID_NDK})
else(NOT $ENV{NDK_ROOT} STREQUAL "")
	set(ANDROID_NDK $ENV{NDK_ROOT})
endif()

message(STATUS "ANDROID_NDK=${ANDROID_NDK}")

if(${ANDROID_NDK} STREQUAL " ")
	message(FATAL_ERROR "Can not found NDK.")
	return()
endif()

#检查NDK中是否有 android.toolchain.cmake 该文件从NDK-R13开始支持。
#if(EXISTS ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
#	set(ANDROID_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
#else()
#    message(FATAL_ERROR "${ANDROID_NDK}/build/cmake/android.toolchain.cmake no such file.")
#    return()
#endif()

set(ANDROID_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CMake/ToolChains/android.toolchain.cmake)

#获取各个平台下NDK make.exe的路径
if(WIN32)
	set(MAKE_PROGRAM_PATH ${ANDROID_NDK}/prebuilt/windows-x86_64/bin/make.exe)
elseif(APPLE)
	set(MAKE_PROGRAM_PATH ${ANDROID_NDK}/prebuilt/darwin-x86_64/bin/make)
elseif(LINUX)
	set(MAKE_PROGRAM_PATH ${ANDROID_NDK}/prebuilt/linux-x86_64/bin/make)
else()
	message( FATAL_ERROR "Unsupported platform, CMake will exit" )
	return()
endif()

message(STATUS "MAKE_PROGRAM_PATH=${MAKE_PROGRAM_PATH}")

if(EXISTS ${MAKE_PROGRAM_PATH})
	set(CMAKE_MAKE_PROGRAM ${MAKE_PROGRAM_PATH})
else()
    message(FATAL_ERROR "Please make sure have make.exe")
    return()
endif()

#设置编译器
set(ANDROID_TOOLCHAIN "gcc")

if(ANDROID_TOOLCHAIN STREQUAL "clang")
	set(ANDROID_STL "c++_static")
else()
	set(ANDROID_STL "gnustl_static")
endif()

#设置android 平台版本
set(ANDROID_PLATFORM "android-21")

#设置Native API Level
set(ANDROID_NATIVE_API_LEVEL 21)

#引用android.toolchain.cmake
include(${ANDROID_TOOLCHAIN_FILE})

