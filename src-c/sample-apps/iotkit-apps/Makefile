all: clean plugin

clean:
	-test -e "subscriber.out" || if [ -f subscriber.out ]; then rm subscriber.out; fi;
	-test -e "publisher.out" || if [ -f publisher.out ]; then rm publisher.out; fi;
	
plugin: *.c *.h 
	cc -g -o subscriber.out iotkit-subscriber-direct.c -I../../edison-lib/libedison/ -I../../edison-lib/cJSON/ ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c -lpaho-mqtt3as -lpthread -ldns_sd -lm -ldl
	cc -g -o publisher.out iotkit-publisher.c -lpaho-mqtt3as -I../../edison-lib/libedison/ -I../../edison-lib/cJSON/ ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c -lpaho-mqtt3as -lpthread -ldns_sd -lm -ldl
    
