project(mvd-parser)
cmake_minimum_required(VERSION 2.8)


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

include(DefineInstallationPaths)
include(ReleaseDebugAutoFlags)


## find dependencies
find_package(Boost 1.41 REQUIRED COMPONENTS system)
find_package(HDF5 REQUIRED)


file(GLOB MVD_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/mvd/*)
file(GLOB MVD_HEADERS_BITS ${CMAKE_CURRENT_SOURCE_DIR}/include/mvd/*/*)



include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${BOOST_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/deps/highFive/include)


aux_source_directory(src SRC_LIST)
add_executable(mvd-tool ${SRC_LIST} ${MVD_HEADERS} ${MVD_HEADERS_BITS})
target_link_libraries(mvd-tool ${HDF5_C_LIBRARIES} ${HDF5_HL_LIBRARIES})



add_subdirectory(tests/unit)

