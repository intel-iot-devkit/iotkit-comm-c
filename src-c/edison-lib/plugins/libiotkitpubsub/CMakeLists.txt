
MESSAGE("Building the mqtt plugin")

INCLUDE_DIRECTORIES(
    ../../libedison
    ../inc/
)

# set the shared library name
SET(IOTKIT_CLIENT_NAME "iotkit-client")

# create the shared library
ADD_LIBRARY(${IOTKIT_CLIENT_NAME} SHARED iotkit-client.c iotkit-client.h)

SET_TARGET_PROPERTIES(${IOTKIT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${IOTKIT_CLIENT_NAME} paho-mqtt3as)
