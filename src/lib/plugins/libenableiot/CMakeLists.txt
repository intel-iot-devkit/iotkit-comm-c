
MESSAGE("Building the enableiot plugin")

INCLUDE_DIRECTORIES(
    ../../iotkit-comm
    ../inc/
    ../../cJSON
)

# set the shared library name
SET(ENABLEIOT_CLIENT_NAME "enableiot-client")
SET(ENABLEIOT_SERVICE_NAME "enableiot-service")

# create the shared library
ADD_LIBRARY(${ENABLEIOT_CLIENT_NAME} SHARED enableiot-client.c enableiot-client.h)
ADD_LIBRARY(${ENABLEIOT_SERVICE_NAME} SHARED enableiot-service.c enableiot-service.h)

SET_TARGET_PROPERTIES(${ENABLEIOT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ENABLEIOT_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${ENABLEIOT_CLIENT_NAME} iotkit curl paho-mqtt3as)
TARGET_LINK_LIBRARIES(${ENABLEIOT_SERVICE_NAME} iotkit curl paho-mqtt3as)
