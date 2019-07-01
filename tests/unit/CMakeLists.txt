if(NOT BUILD_UNIT_TESTS)
  return()
endif()

find_package(Boost 1.41 QUIET REQUIRED COMPONENTS filesystem system
  unit_test_framework)

set(MVD2_TEST_FILE ${PROJECT_SOURCE_DIR}/tests/circuit.mvd2)
set(MVD3_TEST_FILE ${PROJECT_SOURCE_DIR}/tests/circuit.mvd3)

add_definitions(-DMVD2_FILENAME="${MVD2_TEST_FILE}")
add_definitions(-DMVD3_FILENAME="${MVD3_TEST_FILE}")
if(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_COMPILER_IS_CLANG)
  add_definitions(-Wno-unused-local-typedefs)
endif()

## mvd2
add_executable(test_mvd2 tests_mvd2.cpp)
target_link_libraries(test_mvd2 ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES} MVDTool)
add_test(NAME test_parser_mvd2 COMMAND test_mvd2)

# mvd3
add_executable(test_mvd3 tests_mvd3.cpp)
target_link_libraries(test_mvd3 ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES} MVDTool)
add_test(NAME test_parser_mvd3 COMMAND test_mvd3)

