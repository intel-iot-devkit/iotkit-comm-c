
INCLUDE_DIRECTORIES(
    ../../edison-lib/cJSON
    ../../edison-lib/libedison
)

# set the executable names
SET(MQTT_CLIENT_NAME "subscriber")
SET(MQTT_SERVICE_NAME "publisher")

SET(CLIENT_LIST subscriber.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(SERVICE_LIST publisher.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)

ADD_EXECUTABLE(${MQTT_CLIENT_NAME} ${CLIENT_LIST})
ADD_EXECUTABLE(${MQTT_SERVICE_NAME} ${SERVICE_LIST})

SET_TARGET_PROPERTIES(${MQTT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${MQTT_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${MQTT_CLIENT_NAME} paho-mqtt3as pthread dns_sd m dl)
TARGET_LINK_LIBRARIES(${MQTT_SERVICE_NAME} paho-mqtt3as pthread dns_sd m dl)
