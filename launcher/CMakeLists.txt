find_package(DocOpt.CPP REQUIRED)

add_executable(abii abii.cpp)
target_link_libraries(abii PRIVATE docopt_s)

if (BIT32)
	set_target_properties(abii PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif ()

install(TARGETS abii RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
