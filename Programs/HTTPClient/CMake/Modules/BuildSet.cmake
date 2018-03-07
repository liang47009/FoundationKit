# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "FATAL: In-source builds are not allowed. You should create a separate directory for build files.")
endif()

macro(BuildSet)
    # It ensures that when Find*.cmake files included from cmake's Modules dir
    # include another *.cmake file with relative path, that file will be included
    # also from cmake's Modules dir, to not clash with per-project files.
    cmake_policy(SET CMP0017 NEW)

    # Use new behaviour with cmake >= 3.1:
    # Only interpret if() arguments as variables or keywords when unquoted.
    if(CMAKE_VERSION VERSION_GREATER 3.1)
        cmake_policy(SET CMP0054 NEW)
    endif()

    # architecture
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_DIR "64-bit")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH_DIR "32-bit")
    else()
        message(WARN "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")
    endif()
    
    # CMAKE_BUILD_TYPE has precedence over DEBUG_MODE
    # Still supporting DEBUG_MODE for backwards compatibility
    if(NOT CMAKE_BUILD_TYPE)
        if(DEBUG_MODE)
            set(CMAKE_BUILD_TYPE DEBUG)
        else(DEBUG_MODE)
            set(CMAKE_BUILD_TYPE RELEASE)
        endif(DEBUG_MODE)
    endif(NOT CMAKE_BUILD_TYPE)

    # Define other useful variables not defined by CMake
    if(CMAKE_GENERATOR STREQUAL Xcode)
        set (XCODE TRUE)
    elseif(CMAKE_GENERATOR MATCHES Visual)
        set (VS TRUE)
    endif()

    # set compiler options
    include(SetCompilerOptions)
    SetCompilerOptions()

    include(BuildModules)
    BuildModules()
endmacro(BuildSet)