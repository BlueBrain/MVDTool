add_definitions(-DMVD_VERSION_MAJOR=\"${MVDTOOL_VERSION_MAJOR}\" -DMVD_VERSION_MINOR=\"${MVDTOOL_VERSION_MINOR}\")

add_executable(mvd-tool converter.cpp converter.hpp mvd-tool.cpp neuroninfo.hpp)
target_link_libraries(mvd-tool MVDTool HighFive)

install(TARGETS mvd-tool RUNTIME DESTINATION ${BIN_INSTALL_DIR})

