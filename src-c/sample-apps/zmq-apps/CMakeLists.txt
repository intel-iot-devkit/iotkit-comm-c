
INCLUDE_DIRECTORIES(
  ../../edison-lib/cJSON
  ../../edison-lib/libedison
)

# set the executable names
SET(ZMQ_SUB_CLIENT zmqsubclient)
SET(ZMQ_PUB_SERVICE zmqpubservice)
SET(ZMQ_REQ_CLIENT zmqreqclient)
SET(ZMQ_REP_SERVICE zmqrepservice)

# set the source files list
SET(SUB_CLIENT_LIST sample_zmqpubsub-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(PUB_SERVICE_LIST sample_zmqpubsub-service.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(REQ_CLIENT_LIST sample_zmqreqrep-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(REP_SERVICE_LIST sample_zmqreqrep-service.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)

# create the executables
ADD_EXECUTABLE(${ZMQ_SUB_CLIENT} ${SUB_CLIENT_LIST})
ADD_EXECUTABLE(${ZMQ_PUB_SERVICE} ${PUB_SERVICE_LIST})
ADD_EXECUTABLE(${ZMQ_REQ_CLIENT} ${REQ_CLIENT_LIST})
ADD_EXECUTABLE(${ZMQ_REP_SERVICE} ${REP_SERVICE_LIST})

SET_TARGET_PROPERTIES(${ZMQ_SUB_CLIENT} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_PUB_SERVICE} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_REQ_CLIENT} PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(${ZMQ_REP_SERVICE} PROPERTIES LINKER_LANGUAGE C)

# Link the executables
TARGET_LINK_LIBRARIES(${ZMQ_SUB_CLIENT} libzmq.so libpthread.so libdns_sd.so libm.so libdl.so)
TARGET_LINK_LIBRARIES(${ZMQ_PUB_SERVICE} libzmq.so libpthread.so libdns_sd.so libm.so libdl.so)
TARGET_LINK_LIBRARIES(${ZMQ_REQ_CLIENT} libzmq.so libpthread.so libdns_sd.so libm.so libdl.so)
TARGET_LINK_LIBRARIES(${ZMQ_REP_SERVICE} libzmq.so libpthread.so libdns_sd.so libm.so libdl.so)
