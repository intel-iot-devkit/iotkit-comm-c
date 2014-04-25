###############################################################
# Generates the shared library and copies it to plugins folder
###############################################################

# To enable DEBUG flag build the library as
# cc -c -fPIC -DDEBUG zmq-reqrep.c -o zmqreqrep.o

all: setup

clean:
	@if test -d output ; then rm -rf output ; fi
	@if test -f ../libzmqreqrep.so ; then rm ../libzmqreqrep.so ; fi

setup: *.c *.h
	@if ! test -d output; then mkdir output; fi
	cc -c -fPIC zmq-reqrep.c -o ./output/zmqreqrep.o
	cc -shared -o ./output/libzmqreqrep.so ./output/zmqreqrep.o
	cp ./output/libzmqreqrep.so ../

