

SET(MVD2_TEST_FILE ${CMAKE_SOURCE_DIR}/tests/circuit.mvd2)

add_definitions(-DMVD2_FILENAME="${MVD2_TEST_FILE}")

file(GLOB  tests_mvd2_src "*mvd2.cpp")
add_executable(test_mvd2 ${tests_mvd2_src})

target_link_libraries(test_mvd2 ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES})

add_test(NAME test_parser_mvd2 COMMAND test_mvd2)
