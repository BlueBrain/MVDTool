add_library(MVDTool INTERFACE)

target_include_directories(MVDTool INTERFACE
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")
target_include_directories(MVDTool SYSTEM INTERFACE ${Boost_INCLUDE_DIR})
target_link_libraries(MVDTool INTERFACE HighFive)
target_compile_definitions(MVDTool INTERFACE -DH5_USE_BOOST)

if(EXTLIB_FROM_SUBMODULES)
  target_link_libraries(MVDTool INTERFACE sonata_static)
else()
  target_link_libraries(MVDTool INTERFACE sonata::sonata_shared)
endif()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/MVDToolConfig.cmake.in
  ${PROJECT_BINARY_DIR}/MVDToolConfig.cmake
  INSTALL_DESTINATION share/${PROJECT_NAME}/CMake
  )
install(FILES ${PROJECT_BINARY_DIR}/MVDToolConfig.cmake
  DESTINATION share/${PROJECT_NAME}/CMake)

# Generate ${PROJECT_NAME}Targets.cmake; is written after the CMake run
# succeeds. Provides IMPORTED targets when using this project from the install
# tree.
if(MVD_INSTALL_DEV)  # E.g. Python installations dont need this

install(EXPORT ${PROJECT_NAME}Targets FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION share/${PROJECT_NAME}/CMake)

install(TARGETS MVDTool EXPORT ${PROJECT_NAME}Targets
  INCLUDES DESTINATION include)

export(EXPORT ${PROJECT_NAME}Targets
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

endif()
