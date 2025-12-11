include(FetchContent)

if (POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
endif ()

set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "" FORCE)

# ExternalProject to download and build Boost
FetchContent_Declare(docopt URL "https://github.com/docopt/docopt.cpp/archive/refs/tags/v0.6.3.tar.gz"
                     URL_HASH MD5=c6290672c8dae49a01774297a51046fe)
FetchContent_MakeAvailable(docopt)

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

if (BIT32)
	set_target_properties(docopt_o PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	set_target_properties(docopt PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif ()
