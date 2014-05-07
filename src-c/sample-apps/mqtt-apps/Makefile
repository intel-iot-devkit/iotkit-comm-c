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
	@if test -f client.out ; then rm ./client.out ; fi
	@if test -f service.out ; then rm service.out ; fi

setup:
	cc -g -o service.out sample_async_cloud-service.c -I../../edison-lib/libedison/ -I../../edison-lib/cJSON/ ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c -lpaho-mqtt3as -lpthread -ldns_sd -lm -ldl
	cc -g -o client.out sample_async_cloud-client.c -I../../edison-lib/libedison/ -I../../edison-lib/cJSON/ ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c -lpaho-mqtt3as -lpthread -ldns_sd -lm -ldl
