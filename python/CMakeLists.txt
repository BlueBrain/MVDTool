add_subdirectory(pybind11)

pybind11_add_module(mvdtool mvdtool.cpp)
target_link_libraries(mvdtool PUBLIC MVDTool HighFive)
install(TARGETS mvdtool DESTINATION lib/python)
