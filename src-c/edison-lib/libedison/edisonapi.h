
typedef struct _Context {
    char *name;
    char *value;
} Context;

// handle to the client communication plugin
typedef struct _CommClientHandle {
    int (*init)(char *, int, char *, void *);
    int (*send)(char *, Context context);	// int send(topic, context)
    int (*subscribe)(char *);	// int subscribe(topic)
    int (*unsubscribe)(char *);	// int unsubscribe(topic)
    int (*setReceivedMessageHandler)(void (*)(char *, Context)); // int setReceivedMessageHandler(handler) // handler takes 2 parameters
    int (*done)();
    void *handle;	// handle to the dll
} CommClientHandle;

// handle to the service communication plugin
typedef struct _CommServiceHandle {
    int (*init)(char *, int, char *, void *);
    int (*sendTo)(void *, char *, Context context);	// int send(client, message, context) // for example, incase of mqtt... int sendTo(<<mqtt client>>, message, context);
    int (*publish)(char *,Context context); // int publish(message,context)
    int (*manageClient)(void *,Context context); // int manageClient(client,context) // for example, incase of mqtt... int manageClient(<<mqtt client>>, context);
    int (*setReceivedMessageHandler)(void (*)(void *, char *, Context context)); // int setReceivedMessageHandler(handler) // handler takes 3 parameters
    int (*done)();
    void *handle;	// handle to the dll
} CommServiceHandle;


//typedef enum { ADDED, REMOVED, UNKNOWN/home/skothurx/mango/edison-api/src-c/edison-lib/libedison/plugin-interfaces } ServiceStatus;

typedef struct _Prop {
    char *key;
    char *value;
} Property;

// service description
typedef struct _ServiceDescription {
    enum { ADDED, REMOVED, REGISTERED, IN_USE, UNKNOWN } status;
    char *service_name;	    // name of the service
    struct {
	char *name;
	char *protocol; // the protocol
	int numSubTypes;
	char **subTypes;
    } type;
    char *address;
    int port;
    char *comm_params;
    int numProperties;
    Property *properties;
} ServiceDescription, ServiceQuery;

// Create client which returns a CommClientHandle
CommClientHandle *createClient(ServiceQuery *);

// Create service which returns a CommServiceHandle
CommServiceHandle *createService(ServiceDescription *);


void cleanUpClient(CommClientHandle *);
void cleanUpService(CommServiceHandle *);
