# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008"
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles"
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0048 NEW)
endif(COMMAND cmake_policy)

# project name and language
project(BLAKER VERSION 0.7.0)

# check build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# make uppercase string of build type
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

# check build type again
if (NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL)$")
    message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

## set output directory (build/)
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# enable Win32 resource
enable_language(RC)

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # using Clang
        set(CMAKE_C_FLAGS "-static")
        set(CMAKE_CXX_FLAGS "-static")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
        set(CMAKE_C_FLAGS "-static -static-libgcc -static-libstdc++")
        set(CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++")
    elseif (MSVC)
        # replace "/MD" with "/MT" (building without runtime DLLs)
        set(CompilerFlags
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        foreach(CompilerFlags ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
        endforeach()
    endif()
endif()

##############################################################################

subdirs(libqrencode zbar DeleteDirectory microtar)

include_directories(
    .
    DeleteDirectory
    base64
    comp_decomp
    fp_wrap
    katahiromz_pdfium
    libqrencode
    microtar
    mstr)

## ZBar
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
#find_package(ZBar0 REQUIRED)
#include_directories(${ZBAR_INCLUDE_DIR})

set(ZBAR_INCLUDE_DIR zbar/zbar)
set(ZBAR_LIBRARIES zbar)

# OpenCV
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# zlib
find_package(ZLIB)
include_directories(${ZLIB_INCLUDE_DIRS})
if (ZLIB_FOUND)
    add_definitions(-DHAVE_ZLIB)
    message(STATUS "ZLIB found.")
else()
    message(STATUS "ZLIB not found.")
endif()

# bzip2
find_package(BZip2)
include_directories(${BZIP2_INCLUDE_DIR})
if (BZIP2_FOUND)
    add_definitions(-DHAVE_BZLIB)
    message(STATUS "BZIP2 found")
else()
    message(STATUS "BZIP2 not found.")
endif()

# lzma
find_package(LibLZMA)
include_directories(${LIBLZMA_INCLUDE_DIRS})
if (LIBLZMA_FOUND)
    add_definitions(-DHAVE_LZMA)
    message(STATUS "LZMA found")
else()
    message(STATUS "LZMA not found.")
endif()

# executable
add_executable(BLAKER WIN32
    MAboutDlg.cpp
    MBlakerApp.cpp
    MBlakerScreen.cpp
    MChooseOptionDlg.cpp
    MDropSource.cpp
    MPrintHelperEx.cpp
    MScanImagesDlg.cpp
    MScanningMovieDlg.cpp
    MScreenImageDlg.cpp
    MSpecialStatic.cpp
    MStartScreenDlg.cpp
    QRCode.cpp
    tbz.cpp
    BLAKER_res.rc)

# Unicode support
target_compile_definitions(BLAKER PRIVATE -DUNICODE -D_UNICODE)

# link
target_link_libraries(BLAKER comctl32 comdlg32 shlwapi vfw32
    qrencode
    microtar
    DeleteDirectory
    iconv
    ${ZBAR_LIBRARIES}
    ${OpenCV_LIBS}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${LIBLZMA_LIBRARIES})

##############################################################################
