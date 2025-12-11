find_package(DocOpt.CPP REQUIRED)

add_executable(abii abii.cpp)
target_link_libraries(abii PRIVATE docopt_s)

install(TARGETS abii RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
