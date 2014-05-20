###############################################################
# Generates the shared library and copies it to plugins folder
# Copyright (c) 2014, Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU Lesser General Public License,
# version 2.1, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
# more details.
#
###############################################################
all: setup

clean:
	@if test -f subclient.out ; then rm subclient.out ; fi
	@if test -f pubservice.out ; then rm pubservice.out ; fi

setup:
	cc -g -o subclient.out sample_zmqpubsub-client.c -I../../../../../edison-lib/libedison/ -I../../../../../edison-lib/cJSON/ ../../../../../edison-lib/cJSON/cJSON.c ../../../../../edison-lib/libedison/edisonapi.c ../../../../../edison-lib/libedison/edisonMDNS.c -lzmq -lpthread -ldns_sd -lm -ldl
	cc -g -o pubservice.out sample_zmqpubsub-service.c -I../../../../../edison-lib/libedison/ -I../../../../../edison-lib/cJSON/ ../../../../../edison-lib/cJSON/cJSON.c ../../../../../edison-lib/libedison/edisonapi.c ../../../../../edison-lib/libedison/edisonMDNS.c -lzmq -lpthread -ldns_sd -lm -ldl
