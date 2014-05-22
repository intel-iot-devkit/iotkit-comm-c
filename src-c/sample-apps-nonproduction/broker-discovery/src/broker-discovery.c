/*
 ============================================================================
 Name        : broker-discovery.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h> 		// For stdout, stderr
#include <stdlib.h> 	// For exit()
#include <errno.h>		// For errno, EINTR
#include <dns_sd.h>
#include <sys/types.h>
#include <sys/time.h>

typedef union { unsigned char b[2]; unsigned short NotAnInteger; } Opaque16;
static uint32_t opinterface = kDNSServiceInterfaceIndexAny;
static int operation = 'R';
#define LONG_TIME 100000000
static volatile int timeOut = LONG_TIME;
static DNSServiceRef client  = NULL;
static volatile int stopNow = 0;

static void DNSSD_API reg_reply(DNSServiceRef client, const DNSServiceFlags flags, DNSServiceErrorType errorCode,
	const char *name, const char *regtype, const char *domain, void *context)
	{
	(void)client;   // Unused
	(void)flags;    // Unused
	(void)context;  // Unused

	printf("Got a reply for %s.%s%s: ", name, regtype, domain);

	if (errorCode == kDNSServiceErr_NoError)
		{
		printf("Name now registered and active\n");
		if (operation == 'A' || operation == 'U' || operation == 'N') timeOut = 5;
		}
	else if (errorCode == kDNSServiceErr_NameConflict)
		{
		printf("Name in use, please choose another\n");
		exit(-1);
		}
	else
		printf("Error %d\n", errorCode);

	if (!(flags & kDNSServiceFlagsMoreComing)) fflush(stdout);
	}

static void ShowTXTRecord(uint16_t txtLen, const unsigned char *txtRecord)
	{
	const unsigned char *ptr = txtRecord;
	const unsigned char *max = txtRecord + txtLen;
	while (ptr < max)
		{
		const unsigned char *const end = ptr + 1 + ptr[0];
		if (end > max) { printf("<< invalid data >>"); break; }
		if (++ptr < end) printf(" ");   // As long as string is non-empty, begin with a space
		while (ptr<end)
			{
			// We'd like the output to be shell-friendly, so that it can be copied and pasted unchanged into a "dns-sd -R" command.
			// However, this is trickier than it seems. Enclosing a string in double quotes doesn't necessarily make it
			// shell-safe, because shells still expand variables like $foo even when they appear inside quoted strings.
			// Enclosing a string in single quotes is better, but when using single quotes even backslash escapes are ignored,
			// meaning there's simply no way to represent a single quote (or apostrophe) inside a single-quoted string.
			// The only remaining solution is not to surround the string with quotes at all, but instead to use backslash
			// escapes to encode spaces and all other known shell metacharacters.
			// (If we've missed any known shell metacharacters, please let us know.)
			// In addition, non-printing ascii codes (0-31) are displayed as \xHH, using a two-digit hex value.
			// Because '\' is itself a shell metacharacter (the shell escape character), it has to be escaped as "\\" to survive
			// the round-trip to the shell and back. This means that a single '\' is represented here as EIGHT backslashes:
			// The C compiler eats half of them, resulting in four appearing in the output.
			// The shell parses those four as a pair of "\\" sequences, passing two backslashes to the "dns-sd -R" command.
			// The "dns-sd -R" command interprets this single "\\" pair as an escaped literal backslash. Sigh.
			if (strchr(" &;`'\"|*?~<>^()[]{}$", *ptr)) printf("\\");
			if      (*ptr == '\\') printf("\\\\\\\\");
			else if (*ptr >= ' ' ) printf("%c",        *ptr);
			else                   printf("\\\\x%02X", *ptr);
			ptr++;
			}
		}
	}


static DNSServiceErrorType RegisterService(DNSServiceRef *sdRef,
	const char *nam, const char *typ, const char *dom, const char *host, const char *port, int argc, char **argv)
	{
	uint16_t PortAsNumber = atoi(port);
	Opaque16 registerPort = { { PortAsNumber >> 8, PortAsNumber & 0xFF } };
	unsigned char txt[2048] = "";
	unsigned char *ptr = txt;
	int i;

	if (nam[0] == '.' && nam[1] == 0) nam = "";   // We allow '.' on the command line as a synonym for empty string
	if (dom[0] == '.' && dom[1] == 0) dom = "";   // We allow '.' on the command line as a synonym for empty string

	printf("Registering Service %s.%s%s%s", nam[0] ? nam : "<<Default>>", typ, dom[0] ? "." : "", dom);
	if (host && *host) printf(" host %s", host);
	printf(" port %s\n", port);

	if (argc)
		{
		for (i = 0; i < argc; i++)
			{
			const char *p = argv[i];
			*ptr = 0;
			while (*p && *ptr < 255 && ptr + 1 + *ptr < txt+sizeof(txt))
				{
				if      (p[0] != '\\' || p[1] == 0)                       { ptr[++*ptr] = *p;           p+=1; }
				else if (p[1] == 'x' && isxdigit(p[2]) && isxdigit(p[3])) { ptr[++*ptr] = HexPair(p+2); p+=4; }
				else                                                      { ptr[++*ptr] = p[1];         p+=2; }
				}
			ptr += 1 + *ptr;
			}
		ShowTXTRecord(ptr-txt, txt);
		printf("\n");
		}

	return(DNSServiceRegister(sdRef, /* kDNSServiceFlagsAllowRemoteQuery */ 0, opinterface, nam, typ, dom, host, registerPort.NotAnInteger, (uint16_t) (ptr-txt), txt, reg_reply, NULL));
	}

static void myTimerCallBack(void)
	{
	DNSServiceErrorType err = kDNSServiceErr_Unknown;

	if (err != kDNSServiceErr_NoError)
		{
		fprintf(stderr, "DNSService call failed %ld\n", (long int)err);
		stopNow = 1;
		}
	}

static void HandleEvents(void)
	{

	int dns_sd_fd  = client  ? DNSServiceRefSockFD(client) : -1;

	int nfds = dns_sd_fd + 1;
	fd_set readfds;
	struct timeval tv;
	int result;



	while (!stopNow)
		{
		// 1. Set up the fd_set as usual here.
		// This example client has no file descriptors of its own,
		// but a real application would call FD_SET to add them to the set here
		FD_ZERO(&readfds);

		// 2. Add the fd for our client(s) to the fd_set
		if (client ) FD_SET(dns_sd_fd , &readfds);

		// 3. Set up the timeout.
		tv.tv_sec  = timeOut;
		tv.tv_usec = 0;

		result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
		if (result > 0)
			{
			DNSServiceErrorType err = kDNSServiceErr_NoError;
			if      (client  && FD_ISSET(dns_sd_fd , &readfds)) err = DNSServiceProcessResult(client );
			if (err) { fprintf(stderr, "DNSServiceProcessResult returned %d\n", err); stopNow = 1; }
			}
		else if (result == 0)
			myTimerCallBack();
		else
			{
			printf("select() returned %d errno %d %s\n", result, errno, strerror(errno));
			if (errno != EINTR) stopNow = 1;
			}
		}
	}

static void printtimestamp(void)
	{
	struct tm tm;
	int ms;
#ifdef _WIN32
	SYSTEMTIME sysTime;
	time_t uct = time(NULL);
	tm = *localtime(&uct);
	GetLocalTime(&sysTime);
	ms = sysTime.wMilliseconds;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	localtime_r((time_t*)&tv.tv_sec, &tm);
	ms = tv.tv_usec/1000;
#endif
	printf("%2d:%02d:%02d.%03d  ", tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
	}

static void DNSSD_API resolve_reply(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
	const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context)
	{
	union { uint16_t s; u_char b[2]; } port = { opaqueport };
	uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

	(void)client;       // Unused
	(void)ifIndex;      // Unused
	(void)context;      // Unused

	printtimestamp();
	if (errorCode) printf("Error code %d\n", errorCode);
	else
		{
		printf("%s can be reached at %s:%u", fullname, hosttarget, PortAsNumber);
		if (flags) printf(" Flags: %X", flags);
		// Don't show degenerate TXT records containing nothing but a single empty string
		if (txtLen > 1) { printf("\n"); ShowTXTRecord(txtLen, txtRecord); }
		printf("\n");
		}

	if (!(flags & kDNSServiceFlagsMoreComing)) fflush(stdout);
	}


int main(void) {
	Opaque16 registerPort = { { 0x12, 0x34 } };
	DNSServiceErrorType err;

	static const char TXT[] = "\xC" "First String" "\xD" "Second String" "\xC" "Third String";
	printf("Registering Service Test._http._tcp.local.\n");

	err = DNSServiceRegister(&client, 0, opinterface, "Test Http", "_http._tcp.", "", NULL, registerPort.NotAnInteger, sizeof(TXT)-1, TXT, reg_reply, NULL);
	if (!client || err != kDNSServiceErr_NoError)
	{
		fprintf(stderr, "DNSServiceRegister call failed %ld\n", (long int)err);
		return (-1);
	}

	err = DNSServiceResolve(&client, 0, opinterface, "Test Http", "_http._tcp", "local", (DNSServiceResolveReply)resolve_reply, NULL);
	if (!client || err != kDNSServiceErr_NoError)
		{
			fprintf(stderr, "DNSServiceResolve call failed %ld\n", (long int)err);
			return (-1);
		}

	HandleEvents();

	// Be sure to deallocate the DNSServiceRef when you're finished
	if (client ) DNSServiceRefDeallocate(client );

	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}

