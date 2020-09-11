# =============================================================================
# Copyright (C) 2016-2020 Blue Brain Project
#
# See top-level LICENSE file for details.
# =============================================================================

find_package(FindPkgConfig QUIET)

find_path(
  HighFive_PROJ
  NAMES CMakeLists.txt
  PATHS "${PROJECT_SOURCE_DIR}/extlib/HighFive")

find_package_handle_standard_args(HighFive REQUIRED_VARS HighFive_PROJ)

if(NOT HighFive_FOUND)
  find_package(Git 1.8.3 QUIET)
  if(NOT ${GIT_FOUND})
    message(FATAL_ERROR "git not found, clone repository with --recursive")
  endif()
  message(STATUS "Sub-module HighFive missing : running git submodule update --init --recursive")
  execute_process(
    COMMAND
      ${GIT_EXECUTABLE} submodule update --init --recursive --
      ${PROJECT_SOURCE_DIR}/extlib/HighFive
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
else()
  message(STATUS "Using HighFive submodule from ${HighFive_PROJ}")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/extlib/HighFive)
