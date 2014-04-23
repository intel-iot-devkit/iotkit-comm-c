// TODO:


#rm -rf output
#mkdir output
#cc -g -fPIC -o ./output/edison-mqtt_async.o -c ./src/edison-mqtt_async.c -I../inc -I./inc
#cc -g -shared -o ./output/libmqttpubsub.so ./output/edison-mqtt_async.o -lm -I../inc
#rm /usr/lib/libmqttpubsub.so
#cp ./output/libmqttpubsub.so /usr/lib/
#ldconfig
#cc -g -o ./src/sample.out ./src/sample.c -lmqttpubsub -lpaho-mqtt3as -ldl -I./inc
