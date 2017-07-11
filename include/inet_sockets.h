#ifndef _INET_SOCKETS_H_
#define _INET_SOCKETS_H_

#include <sys/socket.h>
#include <netdb.h>

// suggest len of addr str
const unsigned int ADDR_STR_LEN = 4096;

// init socket, and connect to server(host, service)
int inetConnect(const char *host, const char *service, int type);

// init tcp server socket, and listen
int inetListen(const char *service, int backlog, socklen_t *addrlen);

// init tcp/udp server socket, and bind port
int inetBind(const char *service, int type, socklen_t *addrlen);

// get addr str from sockaddr
char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen);


#endif
