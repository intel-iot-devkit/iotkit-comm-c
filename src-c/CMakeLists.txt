# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

# About this project


PROJECT(Edison-C-Library)

MESSAGE("Building the root source C Folder")
# add the sub-directories

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/edison-lib/plugins/)

ADD_SUBDIRECTORY(edison-lib)
ADD_SUBDIRECTORY(sample-apps)

