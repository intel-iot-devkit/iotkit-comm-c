all: clean plugin

clean:
	@test -f "subscriber_d2c.out" || if [ -f subscriber_d2c.out ]; then rm subscriber_d2c.out; fi;
	
plugin: *.c *.h 
	cc -g -o subscriber_d2c.out iotkit-subscriber-direct.c -I../../edison-lib/libedison/ -I../../edison-lib/cJSON/ ../../edison-lib/cJSON/cJSON.c ../../edison-lib/libedison/edisonapi.c ../../edison-lib/libedison/edisonMDNS.c -lpaho-mqtt3as -lpthread -ldns_sd -lm -ldl
