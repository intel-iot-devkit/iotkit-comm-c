
INCLUDE_DIRECTORIES(
  ../../edison-lib/cJSON
  ../../edison-lib/libedison
)

# set the executable names
SET(DASHBOARD dashboard)
SET(SENSOR sensor)
SET(THERMOSTAT thermostat)

# set the source files list
SET(DASHBOARD_LIST dashboard.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(SENSOR_LIST temperatureSensor.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(THERMOSTAT_LIST thermostat.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)

# create the executables
ADD_EXECUTABLE(${DASHBOARD} ${DASHBOARD_LIST})
ADD_EXECUTABLE(${SENSOR} ${SENSOR_LIST})
ADD_EXECUTABLE(${THERMOSTAT} ${THERMOSTAT_LIST})

SET_TARGET_PROPERTIES(${DASHBOARD} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${SENSOR} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${THERMOSTAT} PROPERTIES LINKER_LANGUAGE C)

# Link the executables
TARGET_LINK_LIBRARIES(${DASHBOARD} zmq dns_sd m dl)
TARGET_LINK_LIBRARIES(${SENSOR} zmq dns_sd m dl)
TARGET_LINK_LIBRARIES(${THERMOSTAT} zmq dns_sd m dl)
