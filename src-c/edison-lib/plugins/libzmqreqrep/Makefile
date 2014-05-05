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

# To enable DEBUG flag build the library as
# cc -c -fPIC -DDEBUG zmqreqrep-client.c -o zmqreqrepclient.o
# cc -c -fPIC -DDEBUG zmqreqrep-service.c -o zmqreqrepservice.o

all: setup

clean:
	@if test -d output ; then rm -rf output ; fi
	@if test -f ../libzmqreqrep-client.so ; then rm ../libzmqreqrep-client.so ; fi
	@if test -f ../libzmqreqrep-service.so ; then rm ../libzmqreqrep-service.so ; fi

setup: *.c *.h
	@if ! test -d output; then mkdir output; fi
	cc -c -fPIC zmqreqrep-client.c -o ./output/zmqreqrepclient.o -I../../libedison/
	cc -shared -o ./output/libzmqreqrep-client.so ./output/zmqreqrepclient.o -lzmq
	cp ./output/libzmqreqrep-client.so ../
	cc -c -fPIC zmqreqrep-service.c -o ./output/zmqreqrepservice.o -I../../libedison/
	cc -shared -o ./output/libzmqreqrep-service.so ./output/zmqreqrepservice.o -lzmq
	cp ./output/libzmqreqrep-service.so ../

