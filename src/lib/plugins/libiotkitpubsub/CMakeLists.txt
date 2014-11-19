
MESSAGE("Building the mqtt plugin")

INCLUDE_DIRECTORIES(
    ../../libiotkit-comm
    ../inc/
)

# set the shared library name
SET(IOTKIT_AGENT_CLIENT_NAME "iotkit-agent-client")
SET(IOTKIT_AGENT_SERVICE_NAME "iotkit-agent-service")

# create the shared library
ADD_LIBRARY(${IOTKIT_AGENT_CLIENT_NAME} SHARED iotkit-agent-client.c iotkit-agent-client.h)
ADD_LIBRARY(${IOTKIT_AGENT_SERVICE_NAME} SHARED iotkit-agent-service.c iotkit-agent-service.h)

SET_TARGET_PROPERTIES(${IOTKIT_AGENT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${IOTKIT_AGENT_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${IOTKIT_AGENT_CLIENT_NAME} paho-mqtt3as)
TARGET_LINK_LIBRARIES(${IOTKIT_AGENT_SERVICE_NAME} paho-mqtt3as)
