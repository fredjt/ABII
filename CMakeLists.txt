cmake_minimum_required(VERSION 3.28)
project(ABII LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	message(STATUS "Enabling LTO for non-debug builds")
	set(CMAKE_OPTIMIZE_DEPENDENCIES TRUE)
	add_compile_options("-flto" "-ffat-lto-objects" "-O3")
endif ()

option(BIT32 "32 BIT")
if (BIT32)
	add_compile_definitions(BIT32)
endif ()

option(BUILD_TESTS "Build tests")
if (BUILD_TESTS)
	enable_testing()
endif ()

# CPack setup
set(CPACK_PACKAGE_NAME "abii")
set(CPACK_PACKAGE_VENDOR "Trent Tanchin")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ABII - Application Binary Interface Interceptor")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_GENERATOR "TGZ;DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Trent Tanchin <trent@tanchin.org>")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}")

set(CPACK_SOURCE_IGNORE_FILES
    /build/
    /cmake-build-.*/
    /.git/
    /.idea/
    ~$)

include(CPack)

set(CMAKE_INSTALL_BINDIR "bin" CACHE STRING "Executable install directory")
set(CMAKE_INSTALL_INCLUDEDIR "include/abii" CACHE STRING "Header install directory")
set(CMAKE_INSTALL_LIBDIR "lib" CACHE STRING "Library install directory")

add_subdirectory(launcher)
add_subdirectory(src)
if (BUILD_TESTS)
	add_subdirectory(test)
endif ()
