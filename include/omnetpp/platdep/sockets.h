//=========================================================================
//  PLATDEP/SOCKETS.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_PLATDEP_SOCKETS_H
#define __OMNETPP_PLATDEP_SOCKETS_H

//
// With some care, it's possible to write platform-independent socket code
// using the macros below.
//

#ifdef _WIN32

#include <winsock2.h>
#undef min
#undef max

// Winsock prefixes error codes with "WS"
#define SOCKETERR(x)  WS#x
inline int sock_errno()  {return WSAGetLastError();}

// Shutdown mode constants are named differently in winsock2.h
#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH

typedef int socklen_t;

inline int initsocketlibonce() {
    static bool inited = false;  //FIXME "static" and "inline" conflict!
    if (inited) return 0;
    inited = true;
    WSAData wsaData;
    return WSAStartup(MAKEWORD(2,2), &wsaData);
}


#else
//
// Unix version
//
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET int
inline int initsocketlibonce() {return 0;}
inline void closesocket(int fd) {close(fd);}
inline int sock_errno()  {return errno;}
#define SOCKETERR(x)  x
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

#endif

#endif
