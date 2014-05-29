
MESSAGE("Building the Edison Library")

INCLUDE_DIRECTORIES(
  ../cJSON
)

# Set the shared library name
SET(EDISON_LIBRARY_NAME "iecfapi")

# Create the shared library
ADD_LIBRARY(${EDISON_LIBRARY_NAME} SHARED edisonapi.c edisonMDNS.c ../cJSON/cJSON.c)

SET_TARGET_PROPERTIES(${EDISON_LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${EDISON_LIBRARY_NAME} paho-mqtt3as)
