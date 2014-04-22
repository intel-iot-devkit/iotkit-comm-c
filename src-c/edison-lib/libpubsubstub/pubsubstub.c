#include <stdlib.h>
#include <stdio.h>
#include "pubsub.h"

// stub for send
void send(char *topic, char *message)
{
    printf("pubsubstub: send topic %s message %s\n", topic, message);
}

// stub for unsubscribe
void unsubscribe(char *topic) 
{
    printf("pubsubstub: unsubscribe topic %s\n", topic);
}

// stub for subscribe
void subscribe(char *topic)
{
    printf("pubsubstub: subcribe topic %s\n", topic);
}
