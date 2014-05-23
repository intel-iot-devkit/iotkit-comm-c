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
	@if test -f subscriber.out ; then rm ./subscriber.out ; fi
	@if test -f publisher.out ; then rm publisher.out ; fi

setup:
	cc -g -o publisher.out publisher.c -lpaho-mqtt3as -liecfapi -lpthread -ldns_sd -lm -ldl
	cc -g -o subscriber.out subscriber.c -lpaho-mqtt3as -liecfapi -lpthread -ldns_sd -lm -ldl
