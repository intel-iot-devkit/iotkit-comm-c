
// handle to the communication plugin
typedef struct _CommHandle {
    void (*send)(char *, char *);	// void send(topic, message)
    void (*unsubscribe)(char *);	// void unsubscribe(topic)
    void (*subscribe)(char *);	// void subscribe(topic)
    void *handle;	// handle to the dll
} CommHandle;

// service description
typedef struct _ServiceDescription {
    char *service_name;	    // name of the service
    struct Type {
	char *name;
	char *protocol; // the protocol
	char **subTypes;
    } type;
    int port;
    char *properties;
} ServiceDescription;
     

// Create client which returns a CommHandle
CommHandle *createClient();
void cleanUp(CommHandle *);
