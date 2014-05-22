# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

# About this project


PROJECT(Edison-C-Library)

MESSAGE("Building the root source C Folder")
# add the sub-directories

# Copy the plugins to its directory
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/edison-lib/plugins)

# Copy the sample apps executable to its directory
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/sample-apps)

ADD_SUBDIRECTORY(edison-lib)
ADD_SUBDIRECTORY(sample-apps)

