cmake_minimum_required(VERSION 3.11)

option(EXTLIB_FROM_SUBMODULES "External libs from submodules" OFF)
option(MVD_INSTALL_DEV "Install MVD headers and project exports" ON)

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
message(STATUS "Version: ${MVDTool_VERSION}")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

include(CTest)
include(GNUInstallDirs)
include(ReleaseDebugAutoFlags)

option(BUILD_UNIT_TESTS "Enable or disable unit tests" ON)
option(BUILD_PYTHON_BINDINGS "Build python bindings?" OFF)

## find dependencies
find_package(Boost 1.41 QUIET REQUIRED COMPONENTS system)
find_package(HDF5 QUIET REQUIRED)

include(FetchContent)
if(EXTLIB_FROM_SUBMODULES)
    message("Using dependencies from Submodules. (EXTLIB_FROM_SUBMODULES=ON)")
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/deps/libsonata/CMakeLists.txt")
        message(STATUS "Downloading libsonata")
        FetchContent_Declare(
            libsonata
            GIT_REPOSITORY https://github.com/BlueBrain/libsonata.git
            GIT_TAG        v0.1.16
            GIT_SHALLOW ON
            GIT_SUBMODULES_RECURSE ON
            SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/libsonata
        )
        FetchContent_Populate(libsonata)
    endif()
    add_subdirectory("deps/libsonata" EXCLUDE_FROM_ALL)
else()
    message(STATUS "Submodules not used. Looking for dependencies in the system")
    find_package(HighFive QUIET REQUIRED)
    find_package(sonata QUIET REQUIRED)
endif()

include(PackageConfig)

FILE(GLOB MVDTOOL_HEADERS "include/mvdtool/*.hpp")
FILE(GLOB MVDTOOLE_BITS_HEADERS "include/mvdtool/*/*.hpp")
set(MVDTOOL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Stop on first error
add_compile_options("-Wfatal-errors")


if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

add_subdirectory(src)
add_subdirectory(tests/unit)
