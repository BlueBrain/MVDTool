add_definitions(-DMVD_VERSION_MAJOR=\"${MVDTOOL_VERSION_MAJOR}\" -DMVD_VERSION_MINOR=\"${MVDTOOL_VERSION_MINOR}\")

add_executable(mvd-tool converter.cpp converter.hpp mvd-tool.cpp neuroninfo.hpp ${MVDTOOL_HEADERS} ${MVDTOOL_BITS_HEADERS})
target_link_libraries(mvd-tool PUBLIC MVDTool HighFive)
target_include_directories(mvd-tool PUBLIC ${MVDTOOL_INCLUDE_DIRS} ${HIGHFIVE_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})

install(TARGETS mvd-tool RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
