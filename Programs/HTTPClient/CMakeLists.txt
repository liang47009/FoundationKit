project(HTTPClient)

cmake_minimum_required(VERSION 2.8.12)

set(HTTPCLIENT_VERSION_MAJOR "1")
set(HTTPCLIENT_VERSION_MINOR "0")
set(HTTPCLIENT_VERSION_PATCH "0")
set(HTTPCLIENT_VERSION_EXTRA "")
set(HTTPCLIENT_VERSION "${HTTPCLIENT_VERSION_MAJOR}.${HTTPCLIENT_VERSION_MINOR}")
set(HTTPCLIENT_VERSION_FULL "${HTTPCLIENT_VERSION}.${HTTPCLIENT_VERSION_PATCH}${HTTPCLIENT_VERSION_EXTRA}")
set(LIB_SUFFIX "" CACHE STRING "Takes an empty string or 64. Directory where lib will be installed: lib or lib64")

SET(CMAKE_INSTALL_PREFIX ../prebuilt/) 
#--------------------------------------------------------------------
# Set compiler specific flags
#--------------------------------------------------------------------

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_UNICODE -DUNICODE)
    add_definitions("/EHsc")
    set(CMAKE_CXX_FLAGS "/W4 /MP /sdl /O2")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../FoundationKit
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/curl/include/win32/curl)

file(GLOB_RECURSE SOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/*.cpp")
file(GLOB_RECURSE HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/*.hpp")

add_library(HTTPClient STATIC ${SOURCE_LIST} ${HEADER_LIST})

install (TARGETS HTTPClient DESTINATION lib)
install (FILES ${HEADER_LIST} DESTINATION include)

