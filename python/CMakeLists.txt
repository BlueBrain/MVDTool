include(FetchContent)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11/CMakeLists.txt")
    message(STATUS "Downloading pybind11")
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.10.1
        GIT_SHALLOW ON
        GIT_SUBMODULES_RECURSE ON
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11
    )
    FetchContent_Populate(pybind11)
endif()

add_subdirectory(pybind11)

pybind11_add_module(mvdtool mvdtool.cpp)
target_link_libraries(mvdtool PUBLIC MVDTool HighFive)
install(TARGETS mvdtool DESTINATION lib/python)
