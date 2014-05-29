# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

# About this project


PROJECT(Edison-C-Library)

MESSAGE("Building the root source C Folder")
# add the sub-directories
ADD_SUBDIRECTORY(edison-lib)
ADD_SUBDIRECTORY(sample-apps)

# installedison target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_installedison.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_installedison.cmake"
    IMMEDIATE @ONLY)

add_custom_target(installedison
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_installedison.cmake)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

