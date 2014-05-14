
MESSAGE("Building the mqtt plugin")

INCLUDE_DIRECTORIES(
  ../../cJSON
  ../../libedison
  ../inc/
)

# set the shared library name
SET(MQTT_ASYNC_CLIENT_NAME "mqttpubsub-client")
SET(MQTT_ASYNC_SERVICE_NAME "mqttpubsub-service")

# create the shared library
ADD_LIBRARY(${MQTT_ASYNC_CLIENT_NAME} SHARED edison-mqtt_async-client.c edison-mqtt_async-client.h)
ADD_LIBRARY(${MQTT_ASYNC_SERVICE_NAME} SHARED edison-mqtt_async-service.c edison-mqtt_async-service.h)

SET_TARGET_PROPERTIES(${MQTT_ASYNC_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${MQTT_ASYNC_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${MQTT_ASYNC_CLIENT_NAME} paho-mqtt3as)
TARGET_LINK_LIBRARIES(${MQTT_ASYNC_SERVICE_NAME} paho-mqtt3as)
