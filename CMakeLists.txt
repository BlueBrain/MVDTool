project(MVDTool)
cmake_minimum_required(VERSION 3.0)

include(CTest)

set(MVDTOOL_VERSION_MAJOR 1)
set(MVDTOOL_VERSION_MINOR 5)
set(MVDTOOL_VERSION_PATCH 2)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

include(GNUInstallDirs)
include(ReleaseDebugAutoFlags)


option(MVDTOOL_INSTALL_HIGHFIVE "Install header of dependency HighFive (needed for subprojects)" ON)
option(MVDTOOL_UNIT_TESTS "Enable or disable unit tests" ON)
option(BUILD_PYTHON_BINDINGS "Build python bindings?" OFF)
set(Boost_NO_BOOST_CMAKE TRUE)

## find dependencies
find_package(Boost 1.41 QUIET REQUIRED COMPONENTS system)
find_package(HDF5 QUIET REQUIRED)
find_package(HighFive QUIET REQUIRED)

include(PackageConfig)

FILE(GLOB MVDTOOL_HEADERS "include/mvd/*.hpp")
FILE(GLOB MVDTOOLE_BITS_HEADERS "include/mvd/*/*.hpp")
set(MVDTOOL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(src)

if(BUILD_PYTHON_BINDINGS)
  add_subdirectory(python)
endif()

add_subdirectory(tests/unit)
