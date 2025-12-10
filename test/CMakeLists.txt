add_executable(abii_tests tests.cpp)
target_link_libraries(abii_tests PUBLIC abiinterceptor)
add_test(NAME abii_tests COMMAND abii_tests)

if (BIT32)
	target_compile_options(abii_tests PRIVATE -m32)
	target_link_options(abii_tests PRIVATE -m32)
endif ()
