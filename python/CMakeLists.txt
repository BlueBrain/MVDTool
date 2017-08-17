##############################################
# Requirements
find_package(PythonInterpLibs QUIET)
find_package(Cython QUIET)

option(BUILD_PYTHON_BINDINGS "Build python bindings?" ON)
if(NOT BUILD_PYTHON_BINDINGS)
  return()
endif()

##############################################
# Definitions
set(cymod_src "${CMAKE_CURRENT_SOURCE_DIR}/mvdtool.pyx")
set(default_cybindings_cpp "${CMAKE_CURRENT_SOURCE_DIR}/mvdtool.cpp")
set(cybindings_cpp "${CMAKE_CURRENT_BINARY_DIR}/mvdtool.cpp")
set(local_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(cy_sources
  mvdtool.pyx
  include/boost.pxd
  include/MVD2.pxd
  include/MVD3.pxd
  include/MVD_MVDType.pxd
  include/MVD.pxd
  include/std.pxd)

##############################################
# Rules

# .pyx -> .cpp
if(CYTHON_FOUND)
    add_custom_command(
        OUTPUT ${cybindings_cpp}
        COMMAND PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR} ${CYTHON_EXECUTABLE} ${CYTHON_FLAGS} --fast-fail --cplus -I${local_include_dir} --output-file ${cybindings_cpp} ${cymod_src}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        # No dependencies, except own sources
        DEPENDS ${cy_sources}
        COMMENT "Cythonizing ${cymod_src}"
    )
else()
    # No Cython available, copying the C++ source for the binding from the repo
    configure_file(${default_cybindings_cpp} ${cybindings_cpp})
endif()

# .cpp -> shared_lib
add_definitions("-std=c++11" "-Wfatal-errors" "-Wno-strict-aliasing")
python_add_module(mvdtool ${cybindings_cpp})
target_include_directories(mvdtool PRIVATE ${PYTHON_INCLUDE_DIRS} ${local_include_dir})
target_link_libraries(mvdtool MVDTool)


##############################################
# Python packaging

get_target_property(_include_dirs mvd-tool INCLUDE_DIRECTORIES)
set(_inc_str ${local_include_dir})
foreach(dir ${_include_dirs})
    set(_inc_str "${_inc_str}:${dir}")
endforeach()

# Wheel
add_custom_target(pydist
    COMMAND python setup.py build --build-base=${CMAKE_CURRENT_BINARY_DIR}  build_ext "-I${_inc_str}" bdist_wheel --dist-dir ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${cybindings_cpp}
    COMMENT "Building distributable Python wheel in ${CMAKE_BINARY_DIR}"
)

# Source dist - still requires include dirs
add_custom_target(pysrcdist
    COMMAND python setup.py build --build-base=${CMAKE_CURRENT_BINARY_DIR} sdist --dist-dir ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${cybindings_cpp}
    COMMENT "Building distributable Python wheel in ${CMAKE_BINARY_DIR}"
)


##############################################
# Install
#
set(_default_prefixes "/usr/local" "/usr")
list(FIND _default_prefixes ${CMAKE_INSTALL_PREFIX} _index)
if(${_index} GREATER -1)
    # Default installations install Python to default site-packages
    set(_py_install_prefix ${PYTHON_SITE_PACKAGES})
else()
    set(_py_install_prefix ${CMAKE_INSTALL_PREFIX})
endif()

install(TARGETS mvdtool
        LIBRARY DESTINATION "${_py_install_prefix}" )


##############################################
# TESTS
#
add_test(NAME python_read_mvd2
         COMMAND bash -c "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR} python test/read_mvd2.py"
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(NAME python_read_mvd3
         COMMAND bash -c "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR} python test/read_mvd3.py"
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

