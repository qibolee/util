#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "inet_sockets.h"


int inetConnect(const char *host, const char *service, int type) {
    struct addrinfo hints;
    struct addrinfo *result;
    int sfd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    if (getaddrinfo(host, service, &hints, &result) != 0) {
        errno = ENOSYS; 
        return -1;
    }
    for (auto rp = result; rp != nullptr; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(sfd);
    }
    freeaddrinfo(result);
    
    return rp == nullptr ? -1 : sfd;
}

static int inetPassiveSocket(const char *service, int type, socklen_t *addrlen, bool doListen, int backlog) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, optval;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;
    hints.ai_canonname = nullptr;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(nullptr, service, &hints, &result) != 0) {
        errno = ENOSYS;
        return -1;
    }

    optval = 1;
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }
        if (doListen) {
            if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
                close(sfd);
                freeaddrinfo(result);
                return -1;
            }
        }
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(sfd);
    }
    if (rp != nullptr && doListen) {
        if (listen(sfd, backlog) == -1) {
            freeaddrinfo(result);
            return -1;
        }
    }
    if (rp != nullptr && addrlen != nullptr) {
        *addrlen = rp->ai_addrlen;
    }
    freeaddrinfo(result);

    return rp == nullptr ? -1 : sfd;
}

int inetListen(const char *service, int backlog, socklen_t *addrlen) {

    return inetPassiveSocket(service, SOCK_STREAM, addrlen, true, backlog);
}

int inetBind(const char *service, int type, socklen_t *addrlen) {

    return inetPassiveSocket(service, type, addrlen, false, 0);
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen) {
    char host[NI_MAXHOST], service[NI_MAXSERV];
    if (getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
        snprintf(addrStr, addrStrLen, "[%s, %s]", host, service);
    } else {
        snprintf(addrStr, addrStrLen, "[?UNKNOW?]");
    }
    addrStr[addrStrLen - 1] = '\0';
    return addrStr;
}



