# =============================================================================
# Copyright (C) 2016-2020 Blue Brain Project
#
# See top-level LICENSE file for details.
# =============================================================================

find_package(FindPkgConfig QUIET)

find_path(
  Libsonata_PROJ
  NAMES CMakeLists.txt
  PATHS "${PROJECT_SOURCE_DIR}/extlib/libsonata")

find_package_handle_standard_args(sonata REQUIRED_VARS Libsonata_PROJ)

if(NOT sonata_FOUND)
  find_package(Git 1.8.3 QUIET)
  if(NOT ${GIT_FOUND})
    message(FATAL_ERROR "git not found, clone repository with --recursive")
  endif()
  message(STATUS "Sub-module libsonata missing : running git submodule update --init --recursive")
  execute_process(
    COMMAND
      ${GIT_EXECUTABLE} submodule update --init --recursive --
      ${PROJECT_SOURCE_DIR}/extlib/libsonata
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

message(STATUS "Installing libsonata in build directory ${PROJECT_BINARY_DIR}/extlib/libsonata/install")
execute_process(
  COMMAND
    mkdir build
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extlib/libsonata)
execute_process(
  COMMAND
    cmake .. -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/extlib/libsonata -DEXTLIB_FROM_SUBMODULES:BOOL=ON
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extlib/libsonata/build)
execute_process(
  COMMAND
    make
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extlib/libsonata/build)
execute_process(
  COMMAND
    make install
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extlib/libsonata/build)
message(STATUS "Finished installing libsonata in build directory ${PROJECT_BINARY_DIR}/extlib/libsonata")

set(sonata_DIR "${PROJECT_BINARY_DIR}/extlib/libsonata/share/sonata/CMake")
find_package(sonata QUIET REQUIRED)
