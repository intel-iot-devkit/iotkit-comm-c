###############################################################
# Generates the shared library and copies it to plugins folder
###############################################################

all: setup

clean:
	@if test -d output ; then rm -rf output ; fi
	@if test -f ../libmqttpubsub-client.so ; then rm ../libmqttpubsub-client.so ; fi
	@if test -f ../libmqttpubsub-service.so ; then rm ../libmqttpubsub-service.so ; fi
	
setup: *.c *.h 
	@if ! test -d output; then mkdir output; fi
	cc -g -fPIC -o ./output/edison-mqtt_async-client.o -c edison-mqtt_async-client.c -I../../libedison/
	cc -g -shared -o ./output/libmqttpubsub-client.so ./output/edison-mqtt_async-client.o -lpaho-mqtt3as
	cc -g -fPIC -o ./output/edison-mqtt_async-service.o -c edison-mqtt_async-service.c -I../../libedison/
	cc -g -shared -o ./output/libmqttpubsub-service.so ./output/edison-mqtt_async-service.o -lpaho-mqtt3as
	cp ./output/libmqttpubsub-client.so ..
	cp ./output/libmqttpubsub-service.so ..

