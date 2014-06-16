# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

# About this project


PROJECT(Edison-C-Library)

MESSAGE("Building the root source C Folder")
# add the sub-directories
ADD_SUBDIRECTORY(edison-lib)
ADD_SUBDIRECTORY(sample-apps)

