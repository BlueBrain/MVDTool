project(mvd-parser)
cmake_minimum_required(VERSION 2.8)



include(ctest)


file(GLOB MVD_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/mvd/*)
file(GLOB MVD_HEADERS_BITS ${CMAKE_CURRENT_SOURCE_DIR}/include/mvd/*/*)


find_package(Boost 1.41 REQUIRED COMPONENTS filesystem system unit_test_framework)


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/ )

aux_source_directory(src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${MVD_HEADERS} ${MVD_HEADERS_BITS})



add_subdirectory(tests/unit)

