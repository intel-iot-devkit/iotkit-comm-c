
INCLUDE_DIRECTORIES(
    ../../lib/cJSON
    ../../lib
)

# set the executable names
SET(ENABLEIOT_CLIENT_NAME "enableiotclient")
SET(ENABLEIOT_SERVICE_NAME "enableiotservice")

SET(CLIENT_LIST2 enableiot-subscriber.c ../../lib/cJSON/cJSON.c ../../lib/iotkit-comm/iotkit-comm.c ../../lib/iotkit-comm/iotkit-comm_mdns.c)
SET(SERVICE_LIST2 enableiot-publisher.c ../../lib/cJSON/cJSON.c ../../lib/iotkit-comm/iotkit-comm.c ../../lib/iotkit-comm/iotkit-comm_mdns.c)

ADD_EXECUTABLE(${ENABLEIOT_CLIENT_NAME} ${CLIENT_LIST2})
ADD_EXECUTABLE(${ENABLEIOT_SERVICE_NAME} ${SERVICE_LIST2})

SET_TARGET_PROPERTIES(${ENABLEIOT_CLIENT_NAME} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ENABLEIOT_SERVICE_NAME} PROPERTIES LINKER_LANGUAGE C)

# Link the shared library
TARGET_LINK_LIBRARIES(${ENABLEIOT_CLIENT_NAME} curl pthread dns_sd m dl)
TARGET_LINK_LIBRARIES(${ENABLEIOT_SERVICE_NAME} curl pthread dns_sd m dl)
