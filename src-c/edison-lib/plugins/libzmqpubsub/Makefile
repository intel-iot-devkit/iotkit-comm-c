###############################################################
# Generates the shared library and copies it to plugins folder
###############################################################

# To enable DEBUG flag build the library as
# cc -c -fPIC -DDEBUG zmqpubsub-client.c -o zmqpubsubclient.o
# cc -c -fPIC -DDEBUG zmqpubsub-service.c -o zmqpubsubservice.o

all: setup

clean:
	@if test -d output ; then rm -rf output ; fi
	@if test -f ../libzmqpubsub-client.so ; then rm ../libzmqpubsub-client.so ; fi
	@if test -f ../libzmqpubsub-service.so ; then rm ../libzmqpubsub-service.so ; fi

setup: *.c *.h
	@if ! test -d output; then mkdir output; fi
	cc -c -fPIC zmqpubsub-client.c -o ./output/zmqpubsubclient.o
	cc -shared -o ./output/libzmqpubsub-client.so ./output/zmqpubsubclient.o -lzmq
	cp ./output/libzmqpubsub-client.so ../
	cc -c -fPIC zmqpubsub-service.c -o ./output/zmqpubsubservice.o
	cc -shared -o ./output/libzmqpubsub-service.so ./output/zmqpubsubservice.o -lzmq
	cp ./output/libzmqpubsub-service.so ../
