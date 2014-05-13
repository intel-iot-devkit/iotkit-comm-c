
INCLUDE_DIRECTORIES(
  ../../edison-lib/cJSON
  ../../edison-lib/libedison
)

# set the executable names
SET(MQTT_CLIENT_NAME "mqttasyncclient")
SET(MQTT_SERVICE_NAME "mqttasyncservice")

SET(CLIENT_LIST sample_async_cloud-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(SERVICE_LIST sample_async_cloud-service.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)

ADD_EXECUTABLE(${MQTT_CLIENT_NAME} ${CLIENT_LIST})
ADD_EXECUTABLE(${MQTT_SERVICE_NAME} ${SERVICE_LIST})

SET_TARGET_PROPERTIES(${MQTT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${MQTT_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${MQTT_CLIENT_NAME} libpaho-mqtt3as.so libpthread.so libdns_sd.so libm.so libdl.so)
TARGET_LINK_LIBRARIES(${MQTT_SERVICE_NAME} libpaho-mqtt3as.so libpthread.so libdns_sd.so libm.so libdl.so)
