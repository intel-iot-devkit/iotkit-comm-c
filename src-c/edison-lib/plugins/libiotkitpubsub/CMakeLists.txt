
MESSAGE("Building the mqtt plugin")

INCLUDE_DIRECTORIES(
  ../../cJSON
  ../../libedison
  ../inc/
)

# set the shared library name
SET(IOTKIT_CLIENT_NAME "iotkit-client")
#SET(IOTKIT_SERVICE_NAME "iotkit-service")

# create the shared library
ADD_LIBRARY(${IOTKIT_CLIENT_NAME} SHARED iotkit-client.c iotkit-client.h)
#ADD_LIBRARY(${IOTKIT_SERVICE_NAME} SHARED edison-mqtt_async-service.c edison-mqtt_async-service.h)

SET_TARGET_PROPERTIES(${IOTKIT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
#SET_TARGET_PROPERTIES(${IOTKIT_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${IOTKIT_CLIENT_NAME} paho-mqtt3as)
#TARGET_LINK_LIBRARIES(${IOTKIT_SERVICE_NAME} paho-mqtt3as)
