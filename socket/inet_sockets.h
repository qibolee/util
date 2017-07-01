#ifndef _INET_SOCKETS_H_
#define _INET_SOCKETS_H_

#include <sys/socket.h>
#include <netdb.h>

const int IS_ADDR_STR_LEN = 4096;

int inetConnect(const char *host, const char *service, int type);

int inetListen(const char *service, int backlog, socklen_t *addrlen);

int inetBind(const char *service, int type, socklen_t *addrlen);

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen);



#endif
