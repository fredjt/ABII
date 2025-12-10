include(FetchContent)

# ExternalProject to download and build Boost
FetchContent_Declare(icu URL "https://github.com/unicode-org/icu/releases/download/release-77-1/icu4c-77_1-src.tgz"
                     URL_HASH MD5=bc0132b4c43db8455d2446c3bae58898)
FetchContent_MakeAvailable(icu)

if (BIT32)
	set(BIT32_FLAGS --with-library-bits=32)
endif ()

set(ICU_PREFIX ${icu_BINARY_DIR})

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	set(ICU_BUILDFLAGS STRING "CFLAGS=\"$CFLAGS -fPIC -flto -ffat-lto-objects -O3\" CXXFLAGS=\"$CXXFLAGS -fPIC -flto -ffat-lto-objects -O3\" LDFLAGS=\"$LDFLAGS -flto -ffat-lto-objects -O3\"")
else ()
	set(ICU_BUILDFLAGS STRING "CFLAGS=\"$CFLAGS -fPIC\" CXXFLAGS=\"$CXXFLAGS -fPIC\"")
endif ()

add_custom_command(
		OUTPUT
		${ICU_PREFIX}/lib/libicuuc.a
		${ICU_PREFIX}/lib/libicui18n.a
		COMMAND bash -c "${ICU_BUILDFLAGS} \
		${icu_SOURCE_DIR}/source/configure \
					--enable-static \
					--disable-shared \
					--with-data-packaging=static \
					--disable-extras
					--disable-icuio
			        --disable-tools
					--disable-tests
					--disable-samples
					--disable-icu-config \
					${BIT32_FLAGS}"
		COMMAND make -j
		WORKING_DIRECTORY ${icu_BINARY_DIR}
		COMMENT "Building and installing ICU"
		VERBATIM
		USES_TERMINAL)

# Custom target to invoke the build command
add_custom_target(icu_build ALL DEPENDS
                  ${ICU_PREFIX}/lib/libicuuc.a
                  ${ICU_PREFIX}/lib/libicui18n.a)

set_target_properties(icu_build PROPERTIES COMPILE_FLAGS "-fPIC" LINK_FLAGS "-fPIC")

set(ICU_INCLUDE_DIR ${icu_SOURCE_DIR}/source/common)

foreach (comp i18n uc)
	add_library(ICU::${comp} STATIC IMPORTED GLOBAL)

	set_target_properties(ICU::${comp} PROPERTIES
	                      IMPORTED_LOCATION "${ICU_PREFIX}/lib/libicu${comp}.a"
	                      INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:include>")

	set_target_properties(ICU::${comp} PROPERTIES
	                      INTERFACE_INCLUDE_DIRECTORIES
	                      "$<BUILD_INTERFACE:${ICU_INCLUDE_DIR}>;$<INSTALL_INTERFACE:include/abii/icu>")

	add_dependencies(ICU::${comp} icu_build)
endforeach ()

install(FILES
        ${ICU_PREFIX}/lib/libicuuc.a
        ${ICU_PREFIX}/lib/libicui18n.a
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${ICU_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/icu)
