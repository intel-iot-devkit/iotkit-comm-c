###############################################################
# Generates the shared library and copies it to plugins folder
###############################################################

all: setup

clean:
	@if test -d output ; then rm -rf output ; fi
	@if test -f ../libmqttpubsub.so ; then rm ../libmqttpubsub.so ; fi
	
setup: *.c *.h 
	@if ! test -d output; then mkdir output; fi
	cc -fPIC -o ./output/edison-mqtt_async.o -c edison-mqtt_async.c
	cc -shared -o ./output/libmqttpubsub.so ./output/edison-mqtt_async.o
	cp ./output/libmqttpubsub.so ..

