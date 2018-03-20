project(HTTPClient)

cmake_minimum_required(VERSION 3.0)

# It ensures that when Find*.cmake files included from cmake's Modules dir
# include another *.cmake file with relative path, that file will be included
# also from cmake's Modules dir, to not clash with per-project files.
cmake_policy(SET CMP0017 NEW)

# Use new behaviour with cmake >= 3.1:
# Only interpret if() arguments as variables or keywords when unquoted.
if(CMAKE_VERSION VERSION_GREATER 3.1)
    cmake_policy(SET CMP0054 NEW)
endif()

set(HTTPCLIENT_VERSION_MAJOR "1")
set(HTTPCLIENT_VERSION_MINOR "0")
set(HTTPCLIENT_VERSION_BUILD "0")
set(HTTPCLIENT_VERSION_REVISION "0")
set(HTTPCLIENT_VERSION_FULL "\"${HTTPCLIENT_VERSION_MAJOR}.${HTTPCLIENT_VERSION_MINOR}.${HTTPCLIENT_VERSION_BUILD}.${HTTPCLIENT_VERSION_REVISION}\"")

message(STATUS "Build HTTPClient Version:${HTTPCLIENT_VERSION_FULL}")

# Get build timestamp
string(TIMESTAMP BUILD_TIMESTAMP UTC)
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/HTTPClientVersion.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/HTTPClientVersion.hpp")

#设置库安装路径
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/Binaries/HTTPClient/) 

#设置CMake 模块路径，以便include命令能够正确包含模块
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/)

# Path to where the executable resides under where build is called from
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
if(NOT LIBRARY_OUTPUT_DIRECTORY)
  set(LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
endif()

#C/C++ 标准库版本
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_definitions(-DUNICODE -D_UNICODE)

if(MSVC)
  message(STATUS "======== COMPILER IS MSVC")
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()

  # Enable multiprocessor compilation (some claim this might be harmful?)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /sdl /O2")
  set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt /NODEFAULTLIB:libcmt")
  else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt")
  endif()

  # Use inline debug info (/Z7) format. Or internal error may occur.
  # Errors looks like: "xmemory0(592): error C3130: Internal Compiler Error: failed to write injected code block to PDB"
  foreach(lang C CXX)
      string(REGEX REPLACE "/Z[iI7]" "" CMAKE_${lang}_FLAGS_DEBUG "${CMAKE_${lang}_FLAGS_DEBUG}")
      set(CMAKE_${lang}_FLAGS_DEBUG "${CMAKE_${lang}_FLAGS_DEBUG} /Z7")
  endforeach()

  if(BUILD_SHARED_LIBS)
      add_definitions(-D_USRDLL -D_EXPORT_DLL_ -D_USEGUIDLL -D_USREXDLL -D_USRSTUDIODLL)
  endif()

  add_definitions(-D_WINDOWS -DWIN32 -D_WIN32 -D_UNICODE -DUNICODE /EHsc)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

  # Add the "lib" prefix for generated .lib outputs.
  set(LIB_PREFIX lib)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Use MATCHES for Clang to also include AppleClang
  # Update if necessary
  set(CMAKE_C_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Wcast-align")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual -Wdisabled-optimization")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations -Wmissing-include-dirs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls -Wshadow -Wsign-conversion")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-promo")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdata-sections")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnoexcept")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-null-sentinel")
  endif()

  # -pedantic
  # -Wctor-dtor-privacy
  # -Wformat=2
  # -Wold-style-cast
  # -Wundef
  # -Wstrict-overflow=5
  # -Wswitch-default
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

  if(DEBUG_STL)
    message("Enabling STL debug mode for the debug build.")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
  endif()

 # add_definitions(-DUSE_FILE32API)

  # When building with "make", "lib" prefix will be added automatically by
  # the build tool.
  set(LIB_PREFIX)
else()
  if (NOT IOS)
    message(FATAL_ERROR "Unsupported compiler!")
  endif()
endif()

if(ANDROID)
  set(PLATFORM_FOLDER "ios")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GXX_EXPERIMENTAL_CXX0X__ -D_FILE_OFFSET_BITS=64")
  if(NOT CMAKE_ANDROID_ARCH_ABI)
    set(CMAKE_ANDROID_ARCH_ABI ${ANDROID_ABI})
  endif()
endif()

if(IOS)
  set(PLATFORM_FOLDER "ios")
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../FoundationKit
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/curl/include/win32/curl)


file(GLOB_RECURSE HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/*.hpp")
file(GLOB_RECURSE SOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/*.cpp")
#or
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/ SOURCE_LIST)

add_library(HTTPClient STATIC ${SOURCE_LIST} ${HEADER_LIST})
set_target_properties(HTTPClient
    PROPERTIES
    PUBLIC_HEADER "${HEADER_LIST}")

install(TARGETS HTTPClient
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION prebuilt/
  PUBLIC_HEADER DESTINATION include/${PLATFORM_FOLDER}/HTTPClient/)

#install (FILES ${HEADER_LIST} DESTINATION include)


message(STATUS "System name       : ${CMAKE_SYSTEM_NAME}")
message(STATUS "Project Name      : ${PROJECT_NAME}")
message(STATUS "Project directory : ${PROJECT_SOURCE_DIR}")
message(STATUS "Build called from : ${PROJECT_BINARY_DIR}")
message(STATUS "Executables in    : ${EXECUTABLE_OUTPUT_PATH}")
message(STATUS "Libraries in      : ${LIBRARY_OUTPUT_DIRECTORY}")

message(STATUS "CMAKE_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}")


