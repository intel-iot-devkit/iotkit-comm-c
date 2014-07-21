
INCLUDE_DIRECTORIES(
  ../../edison-lib/cJSON
  ../../edison-lib/libedison
  ../inc/
)

# set the source files list
SET(IOTKITPUBSUB_CLIENT_PUBLISH_FAIL test_iotkitpubsub_client_publish_fail.c ../../edison-lib/plugins/libiotkitpubsub/iotkit-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(IOTKITPUBSUB_CLIENT_PUBLISH_SUCCESS test_iotkitpubsub_client_publish_success.c ../../edison-lib/plugins/libiotkitpubsub/iotkit-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(IOTKITPUBSUB_CLIENT_CONNECT_SUCCESS test_iotkitpubsub_client_connect_success.c ../../edison-lib/plugins/libiotkitpubsub/iotkit-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(IOTKITPUBSUB_CLIENT_SUBSCRIBE_FAIL test_iotkitpubsub_client_subscribe_fail.c ../../edison-lib/plugins/libiotkitpubsub/iotkit-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)
SET(IOTKITPUBSUB_CLIENT_SUBSCRIBE_SUCCESS test_iotkitpubsub_client_subscribe_success.c ../../edison-lib/plugins/libiotkitpubsub/iotkit-client.c ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c)

# create the executables
ADD_EXECUTABLE(iotkitpubsub_client_publish_fail ${IOTKITPUBSUB_CLIENT_PUBLISH_FAIL})
ADD_EXECUTABLE(iotkitpubsub_client_publish_success ${IOTKITPUBSUB_CLIENT_PUBLISH_SUCCESS})
ADD_EXECUTABLE(iotkitpubsub_client_connect_success ${IOTKITPUBSUB_CLIENT_CONNECT_SUCCESS})
ADD_EXECUTABLE(iotkitpubsub_client_subscribe_fail ${IOTKITPUBSUB_CLIENT_SUBSCRIBE_FAIL})
ADD_EXECUTABLE(iotkitpubsub_client_subscribe_success ${IOTKITPUBSUB_CLIENT_SUBSCRIBE_SUCCESS})

SET_TARGET_PROPERTIES(iotkitpubsub_client_publish_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(iotkitpubsub_client_publish_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(iotkitpubsub_client_connect_success PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(iotkitpubsub_client_subscribe_fail PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(iotkitpubsub_client_subscribe_success PROPERTIES LINKER_LANGUAGE C)

# Link the executables
TARGET_LINK_LIBRARIES(iotkitpubsub_client_publish_fail paho-mqtt3as pthread dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(iotkitpubsub_client_publish_success paho-mqtt3as pthread dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(iotkitpubsub_client_connect_success paho-mqtt3as pthread dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(iotkitpubsub_client_subscribe_fail paho-mqtt3as pthread dns_sd m dl gcov)
TARGET_LINK_LIBRARIES(iotkitpubsub_client_subscribe_success paho-mqtt3as pthread dns_sd m dl gcov)

# Add the Test
ADD_TEST(iotkitpubsub_client_publish_fail ${CTEST_BINARY_DIRECTORY}/iotkitpubsub_client_publish_fail)
ADD_TEST(iotkitpubsub_client_publish_success ${CTEST_BINARY_DIRECTORY}/iotkitpubsub_client_publish_success)
ADD_TEST(iotkitpubsub_client_connect_success ${CTEST_BINARY_DIRECTORY}/iotkitpubsub_client_connect_success)
ADD_TEST(iotkitpubsub_client_subscribe_fail ${CTEST_BINARY_DIRECTORY}/iotkitpubsub_client_subscribe_fail)
ADD_TEST(iotkitpubsub_client_subscribe_success ${CTEST_BINARY_DIRECTORY}/iotkitpubsub_client_subscribe_success)
