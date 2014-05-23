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
	@if test -f ./libiecfapi.so ; then rm ./libiecfapi.so ; fi
	
setup: *.c *.h 
	cc -g -fPIC -c edisonapi.c edisonMDNS.c ../cJSON/cJSON.c -I../cJSON
	cc -g -shared -o ./libiecfapi.so edisonapi.o edisonMDNS.o cJSON.o -lpaho-mqtt3as

install:
	mkdir /usr/include/iecfapi/
	cp ../cJSON/cJSON.h /usr/include/iecfapi/
	cp edisonapi.h /usr/include/iecfapi/
	cp edisonMDNS.h /usr/include/iecfapi/
	cp util.h /usr/include/iecfapi/
	cp libiecfapi.so /usr/lib/
	ldconfig 

uninstall:
	rm -rf /usr/include/iecfapi/
	rm /usr/lib/libiecfapi.so
	ldconfig

