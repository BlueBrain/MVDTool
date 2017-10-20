project(MVDTool)
cmake_minimum_required(VERSION 3.0)

enable_testing()

set(MVDTOOL_VERSION_MAJOR 1)
set(MVDTOOL_VERSION_MINOR 3)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

include(DefineInstallationPaths)
include(ReleaseDebugAutoFlags)


option(MVDTOOL_INSTALL_HIGHFIVE "Install header of dependency HighFive (needed for subprojects)" ON)
option(MVDTOOL_UNIT_TESTS "Enable or disable unit tests" ON)
set(Boost_NO_BOOST_CMAKE TRUE)

## find dependencies
find_package(Boost 1.41 QUIET REQUIRED COMPONENTS system)
find_package(HDF5 QUIET REQUIRED)
find_package(HighFive QUIET)

include(PackageConfig)

add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(tests/unit)
