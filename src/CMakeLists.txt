add_library(utils STATIC
            ArgPrinter.tpp
            ArgPrinterArray.tpp
            ArgPrinterFunction.tpp
            ArgPrinterPointer.tpp
            custom_printers.h
            Logger.cpp Logger.h
            libabii.cpp libabii.h
            utils.h)

set(public_headers
    ArgPrinter.tpp
    ArgPrinterArray.tpp
    ArgPrinterFunction.tpp
    ArgPrinterPointer.tpp
    libabii.h
    Logger.h
    utils.h)
set_target_properties(utils PROPERTIES PUBLIC_HEADER "${public_headers}")

find_package(ICU COMPONENTS i18n uc)

target_include_directories(utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(utils PUBLIC ICU::i18n ICU::uc)
set_target_properties(utils PROPERTIES COMPILE_FLAGS "-fPIC" LINK_FLAGS "-fPIC")

add_library(abiinterceptor STATIC initfini.cpp)
target_link_libraries(abiinterceptor PUBLIC utils)

set_target_properties(abiinterceptor PROPERTIES COMPILE_FLAGS "-fPIC" LINK_FLAGS "-fPIC")

if (BIT32)
	set_target_properties(abiinterceptor PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	set_target_properties(utils PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif ()


add_library(libabii INTERFACE)

target_link_libraries(libabii INTERFACE
                      utils

                      "LINKER:--whole-archive"
                      abiinterceptor
                      "LINKER:--no-whole-archive"

                      "LINKER:--start-group"
                      ICU::uc ICU::i18n
                      "LINKER:--end-group")

add_library(abii::abii ALIAS libabii)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/abiiConfigVersion.cmake"
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion)

configure_package_config_file(
		abiiConfig.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}/abiiConfig.cmake"
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/abii)

install(TARGETS utils abiinterceptor libabii
        EXPORT abiiTargets
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/abiiConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/abiiConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/abii)

install(EXPORT abiiTargets
        FILE abiiTargets.cmake
        NAMESPACE abii::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/abii)
