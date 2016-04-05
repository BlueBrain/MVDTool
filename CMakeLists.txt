project(MVDTool)
cmake_minimum_required(VERSION 2.8)

enable_testing()

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

include(DefineInstallationPaths)
include(ReleaseDebugAutoFlags)


option(UNIT_TESTS "Enable or disable unit tests" ON)
set(Boost_NO_BOOST_CMAKE TRUE)

## find dependencies
find_package(Boost 1.41 REQUIRED COMPONENTS system)
find_package(HDF5 REQUIRED)


file(GLOB MVD_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/mvd/*)
file(GLOB MVD_HEADERS_BITS ${CMAKE_CURRENT_SOURCE_DIR}/include/mvd/*/*)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/deps/highfive/include)
include_directories(SYSTEM ${Boost_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS})
add_definitions( -DH5_USE_BOOST -DMVD_VERSION_MAJOR=\"${VERSION_MAJOR}\" -DMVD_VERSION_MINOR=\"${VERSION_MINOR}\" )

aux_source_directory(src SRC_LIST)
add_executable(mvd-tool ${SRC_LIST} ${MVD_HEADERS_BITS} ${MVD_HEADERS})
target_link_libraries(mvd-tool ${HDF5_C_LIBRARIES} ${HDF5_HL_LIBRARIES})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/mvd DESTINATION ${INCLUDE_INSTALL_DIR})
install(TARGETS mvd-tool RUNTIME DESTINATION ${BIN_INSTALL_DIR})

# cmake target install
if(CMAKE_MAJOR_VERSION GREATER 2) # No export(EXPORT) in CMake 2
  # Generate ${PROJECT_NAME}Targets.cmake; is written after the CMake run
  # succeeds. Provides IMPORTED targets when using this project from the install
  # tree.

  add_library(MVDTool INTERFACE)
  target_link_libraries(MVDTool INTERFACE ${HDF5_LIBRARIES})
  target_include_directories(MVDTool INTERFACE
	  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/highfive/include>")
  target_include_directories(MVDTool SYSTEM INTERFACE
	  ${Boost_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS})

  target_compile_definitions(MVDTool INTERFACE
	  -DH5_USE_BOOST
	  -DMVD_VERSION_MAJOR=\"${VERSION_MAJOR}\"
	  -DMVD_VERSION_MINOR=\"${VERSION_MINOR}\")

  install(EXPORT ${PROJECT_NAME}Targets FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION share/${PROJECT_NAME}/CMake)

  export(EXPORT ${PROJECT_NAME}Targets
    FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
    )

  install(TARGETS MVDTool EXPORT ${PROJECT_NAME}Targets
  	INCLUDES DESTINATION include)
endif()

add_subdirectory(tests/unit)
