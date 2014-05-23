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
	@if test -f reqclient.out ; then rm reqclient.out ; fi
	@if test -f repservice.out ; then rm repservice.out ; fi

setup:
	cc -o subclient.out sample_zmqpubsub-client.c -lzmq -liecfapi -lpthread -ldns_sd -lm -ldl
	cc -o pubservice.out sample_zmqpubsub-service.c -lzmq -liecfapi -lpthread -ldns_sd -lm -ldl
	cc -o reqclient.out sample_zmqreqrep-client.c -lzmq -liecfapi -lpthread -ldns_sd -lm -ldl
	cc -o repservice.out sample_zmqreqrep-service.c -lzmq -liecfapi -lpthread -ldns_sd -lm -ldl