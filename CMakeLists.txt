cmake_minimum_required(VERSION 3.0)

if(NOT MVDTool_VERSION)
    execute_process(COMMAND git describe --tags
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_VERSION_FAILED
                    OUTPUT_VARIABLE GIT_PKG_VERSION_FULL
                    ERROR_VARIABLE GIT_VERSION_ERROR
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(GIT_VERSION_FAILED)
      message(
        FATAL_ERROR
          "Could not retrieve version from command 'git describe --tags'\n"
          ${GIT_VERSION_ERROR})
    endif()

    # keep last line of command output
    string(REPLACE "\n"
                   ";"
                   GIT_PKG_VERSION_FULL
                   "${GIT_PKG_VERSION_FULL}")
   list(GET GIT_PKG_VERSION_FULL -1 MVDTool_VERSION)
endif()

# keep MAJOR.MINOR.PATCH
string(REGEX
       REPLACE "v?([0-9]+\\.[0-9]+\\.[0-9]+).*"
               "\\1"
               MVDTool_VERSION
               "${MVDTool_VERSION}")

project(MVDTool VERSION ${MVDTool_VERSION})

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

option(EXTLIB_FROM_SUBMODULES "Use Git submodules for header-only dependencies" OFF)

include(CTest)
include(GNUInstallDirs)
include(ReleaseDebugAutoFlags)

option(BUILD_UNIT_TESTS "Enable or disable unit tests" ON)
option(BUILD_PYTHON_BINDINGS "Build python bindings?" OFF)
set(Boost_NO_BOOST_CMAKE TRUE)

## find dependencies
find_package(Boost 1.41 QUIET REQUIRED COMPONENTS system)
find_package(HDF5 QUIET REQUIRED)
find_package(HighFive QUIET)
if(NOT HighFive_FOUND AND EXTLIB_FROM_SUBMODULES)
    include(AddHighFiveSubmodule)
endif()
find_package(sonata QUIET)
if(NOT sonata_FOUND AND EXTLIB_FROM_SUBMODULES)
    include(AddLibsonataSubmodule)
endif()


include(PackageConfig)

FILE(GLOB MVDTOOL_HEADERS "include/mvdtool/*.hpp")
FILE(GLOB MVDTOOLE_BITS_HEADERS "include/mvdtool/*/*.hpp")
set(MVDTOOL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("-Wfatal-errors")


if(BUILD_PYTHON_BINDINGS)
  add_subdirectory(python)
endif()

add_subdirectory(src)
add_subdirectory(tests/unit)
