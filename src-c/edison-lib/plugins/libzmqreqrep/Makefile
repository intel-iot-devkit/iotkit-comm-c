###############################################################
# Generates the shared library and copies it to plugins folder
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
	cc -c -fPIC zmqreqrep-client.c -o ./output/zmqreqrepclient.o
	cc -shared -o ./output/libzmqreqrep-client.so ./output/zmqreqrepclient.o -lzmq
	cp ./output/libzmqreqrep-client.so ../
	cc -c -fPIC zmqreqrep-service.c -o ./output/zmqreqrepservice.o
	cc -shared -o ./output/libzmqreqrep-service.so ./output/zmqreqrepservice.o -lzmq
	cp ./output/libzmqreqrep-service.so ../

