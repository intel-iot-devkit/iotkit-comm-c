all: clean plugin

clean:
	@test -e "subscriber.out" || if [ -f subscriber.out ]; then rm subscriber.out; fi;
	@test -e "publisher.out" || if [ -f publisher.out ]; then rm publisher.out; fi;
	
plugin: *.c 
	cc -g -o subscriber.out iotkit-subscriber.c -lpaho-mqtt3as -liecfapi -lpthread -ldns_sd -lm -ldl
	cc -g -o publisher.out iotkit-publisher.c -lpaho-mqtt3as -liecfapi -lpthread -ldns_sd -lm -ldl
