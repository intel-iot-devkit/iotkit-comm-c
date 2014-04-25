###############################################################
# Generates the shared library and copies it to plugins folder
###############################################################

# To enable DEBUG flag build the library as
# cc -c -fPIC -DDEBUG zmq-pubsub.c -o zmqpubsub.o

all: setup

clean:
	@if test -d output ; then rm -rf output ; fi
	@if test -f ../libzmqpubsub.so ; then rm ../libzmqpubsub.so ; fi

setup: *.c *.h
	@if ! test -d output; then mkdir output; fi
	cc -c -fPIC zmq-pubsub.c -o ./output/zmqpubsub.o
	cc -shared -o ./output/libzmqpubsub.so ./output/zmqpubsub.o
	cp ./output/libzmqpubsub.so ../

