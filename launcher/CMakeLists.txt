add_executable(abii abii.cpp CLI11.hpp)
target_compile_definitions(abii PRIVATE PROJECT_VERSION="${PROJECT_VERSION}")

install(TARGETS abii RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
