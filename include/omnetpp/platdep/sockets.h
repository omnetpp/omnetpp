//=========================================================================
//  PLATDEP/SOCKETS.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2005
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PLATDEP_SOCKETS_H
#define __PLATDEP_SOCKETS_H

//
// With some care, it's possible to write platform-independent socket code
// using the macros below.
//
// IMPORTANT: this header needs to be included BEFORE <omnetpp.h>, because
// of timeval definitions in timeutil.h (see more info in timeutil.h)
//

#ifdef __PLATDEP_TIMEUTIL_H
# error "#include <platdep/sockets.h> must precede <omnetpp.h> (and <platdep/timeutil.h> if present)"
#endif

#ifdef _WIN32
//
// Winsock version
//

// include <winsock.h> or <winsock2.h> (mutually exclusive) if neither has been included yet
#if !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_)
# ifndef WANT_WINSOCK2
#  include <winsock.h>
# else
#  include <winsock2.h>
# endif
#endif
#undef min
#undef max

// Winsock prefixes error codes with "WS"
#define SOCKETERR(x)  WS#x
inline int sock_errno()  {return WSAGetLastError();}

// Shutdown mode constants are named differently (and only in winsock2.h
// which we don't want to pull in)
#define SHUT_RD   0x00  // as SD_RECEIVE in winsock2.h
#define SHUT_WR   0x01  // as SD_SEND in winsock2.h
#define SHUT_RDWR 0x02  // as SD_BOTH in winsock2.h

typedef int socklen_t;

inline int initsocketlibonce() {
    static bool inited = false;  //FIXME "static" and "inline" conflict!
    if (inited) return 0;
    inited = true;
    WSAData wsaData;
#ifdef _WINSOCKAPI_
    return WSAStartup(MAKEWORD(1,1), &wsaData);
#else
    return WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}


#else
//
// Unix version
//
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
