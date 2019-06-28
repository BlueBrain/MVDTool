cmake_minimum_required(VERSION 3.0)

# get version from Python package
find_package(PythonInterp REQUIRED)
execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py --version
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE PY_VERSION_FAILED
                OUTPUT_VARIABLE PY_PKG_VERSION_FULL
                ERROR_VARIABLE PY_VERSION_ERROR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(PY_VERSION_FAILED)
  message(
    FATAL_ERROR
      "Could not retrieve version from command '${PYTHON_EXECUTABLE} setup.py --version'\n"
      ${PY_VERSION_ERROR})
endif()
# keep last line of command output
string(REPLACE "\n"
               ";"
               PY_PKG_VERSION_FULL
               "${PY_PKG_VERSION_FULL}")
list(GET PY_PKG_VERSION_FULL -1 PY_PKG_VERSION_FULL)
# keep MAJOR.MINOR.PATCH
string(REGEX
       REPLACE "([0-9]+\\.[0-9]+\\.[0-9]+).*"
               "\\1"
               PY_PKG_VERSION
               "${PY_PKG_VERSION_FULL}")

project(MVDTool VERSION ${PY_PKG_VERSION})

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

include(CTest)
include(GNUInstallDirs)
include(PackageConfig)
include(ReleaseDebugAutoFlags)

option(BUILD_UNIT_TESTS "Enable or disable unit tests" ON)
option(BUILD_PYTHON_BINDINGS "Build python bindings?" OFF)
set(Boost_NO_BOOST_CMAKE TRUE)

## find dependencies
find_package(Boost 1.41 QUIET REQUIRED COMPONENTS system)
find_package(HDF5 QUIET REQUIRED)
find_package(HighFive QUIET REQUIRED)

FILE(GLOB MVDTOOL_HEADERS "include/mvd/*.hpp")
FILE(GLOB MVDTOOLE_BITS_HEADERS "include/mvd/*/*.hpp")
set(MVDTOOL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(BUILD_PYTHON_BINDINGS)
  add_subdirectory(python)
endif()

add_subdirectory(src)
add_subdirectory(tests/unit)
