##############################################
# Requirements
find_package(PythonInterpLibs)
find_package(Cython)

option(BUILD_PYTHON_BINDINGS "Build python bindings?" ON)
if(BUILD_PYTHON_BINDINGS)

##############################################
# Definitions
set(cymod_src "${CMAKE_CURRENT_SOURCE_DIR}/mvdtool.pyx")
set(cybindings_cpp "${CMAKE_CURRENT_SOURCE_DIR}/mvdtool.cpp")
FILE(GLOB cy_srcs *.pyx *.pxi includes/*.pxd)
get_target_property(_libs mvd-tool LINK_LIBRARIES)


# Rebuild Cython?
option(REBUILD_PYTHON_BINDINGS "Rebuild Python bindings from Cython sources" OFF)

# Checking requirements
if(REBUILD_PYTHON_BINDINGS OR (NOT EXISTS ${cybindings_cpp}))
    if(NOT CYTHON_FOUND)
        message(FATAL_ERROR "Cython is required to rebuild python bindings but was not found" )
    endif()
    message(WARNING "Will rebuild Cython bindings!")
    file(REMOVE ${cybindings_cpp})
endif()

# .pyx -> .cpp
add_custom_command(
    OUTPUT ${cybindings_cpp}
    COMMAND PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR} ${CYTHON_EXECUTABLE} ${CYTHON_FLAGS} --fast-fail --cplus -I${CMAKE_CURRENT_SOURCE_DIR}/includes --output-file ${cybindings_cpp} ${cymod_src}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    # No dependencies, except own sources
    DEPENDS ${cy_srcs}
    COMMENT "Cythonizing ${cymod_src}"
 )

# .cpp -> shared_lib
add_definitions("-std=c++11" "-Wfatal-errors" "-Wno-strict-aliasing")
python_add_module(mvdtool ${cybindings_cpp})
target_include_directories(mvdtool PUBLIC ${PYTHON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_link_libraries(mvdtool ${_libs})


#################################
# Install
install(TARGETS mvdtool
        LIBRARY DESTINATION "${PYTHON_SITE_PACKAGES}" )


# #####################################
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

#########################################################
endif() # /BUILD_PYTHON_BINDINGS
