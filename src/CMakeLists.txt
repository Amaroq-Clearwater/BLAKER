# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008"
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles"
##############################################################################

include_directories(
    .
    ../DeleteDirectory
    ../SetDlgItemDouble
    ../base64
    ../comp_decomp
    ../fp_wrap
    ../katahiromz_pdfium
    ../libqrencode
    ../microtar
    ../mstr)

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

## zlib
#find_package(ZLIB)
#include_directories(${ZLIB_INCLUDE_DIRS})
#if (ZLIB_FOUND)
#    add_definitions(-DHAVE_ZLIB)
#    message(STATUS "ZLIB found.")
#else()
#    message(STATUS "ZLIB not found.")
#endif()

# bzip2
find_package(BZip2)
include_directories(${BZIP2_INCLUDE_DIR})
if (BZIP2_FOUND)
    add_definitions(-DHAVE_BZLIB)
    message(STATUS "BZIP2 found")
else()
    message(STATUS "BZIP2 not found.")
endif()

## lzma
#find_package(LibLZMA)
#include_directories(${LIBLZMA_INCLUDE_DIRS})
#if (LIBLZMA_FOUND)
#    add_definitions(-DHAVE_LZMA)
#    message(STATUS "LZMA found")
#else()
#    message(STATUS "LZMA not found.")
#endif()

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
    MSettingsDlg.cpp
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
    ${BZIP2_LIBRARIES}
    #${ZLIB_LIBRARIES}
    #${LIBLZMA_LIBRARIES}
)

##############################################################################
