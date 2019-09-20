if(NOT BUILD_UNIT_TESTS)
  return()
endif()

find_package(Boost 1.41 QUIET REQUIRED COMPONENTS filesystem system
  unit_test_framework)



if(NOT Boost_USE_STATIC_LIBS)
  add_definitions(-DBOOST_TEST_DYN_LINK=TRUE)
endif()


set(MVD2_TEST_FILE ${PROJECT_SOURCE_DIR}/tests/circuit.mvd2)
set(MVD3_TEST_FILE ${PROJECT_SOURCE_DIR}/tests/circuit.mvd3)
set(SONATA_TEST_FILE ${PROJECT_SOURCE_DIR}/tests/sonata.h5)
set(SONATA_TEST_FILE_ALTERNATIVE ${PROJECT_SOURCE_DIR}/tests/sonata_alt.h5)

add_definitions(-DMVD2_FILENAME="${MVD2_TEST_FILE}")
add_definitions(-DMVD3_FILENAME="${MVD3_TEST_FILE}")
add_definitions(-DSONATA_FILENAME="${SONATA_TEST_FILE}")
add_definitions(-DSONATA_FILENAME_ALTERNATIVE="${SONATA_TEST_FILE_ALTERNATIVE}")
if(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_COMPILER_IS_CLANG)
  add_definitions(-Wno-unused-local-typedefs)
endif()

## mvd2
add_executable(test_mvd2 tests_mvd2.cpp)
target_link_libraries(test_mvd2 Boost::unit_test_framework MVDTool)
add_test(NAME test_parser_mvd2 COMMAND test_mvd2)

# mvd3
add_executable(test_mvd3 tests_mvd3.cpp)
target_link_libraries(test_mvd3 Boost::unit_test_framework MVDTool)
add_test(NAME test_parser_mvd3 COMMAND test_mvd3)

# sonata
add_executable(test_sonata tests_sonata.cpp)
target_link_libraries(test_sonata Boost::unit_test_framework MVDTool)
add_test(NAME test_parser_sonata COMMAND test_sonata)
