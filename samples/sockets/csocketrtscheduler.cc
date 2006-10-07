//=========================================================================
//  CSOCKETRTSCHEDULER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2005
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "csocketrtscheduler.h"

Register_Class(cSocketRTScheduler);


inline std::ostream& operator<<(std::ostream& ev, const timeval& tv)
{
    return ev << (unsigned long)tv.tv_sec << "s" << tv.tv_usec << "us";
}

//---

cSocketRTScheduler::cSocketRTScheduler() : cScheduler()
{
    listenerSocket = INVALID_SOCKET;
    connSocket = INVALID_SOCKET;
}

cSocketRTScheduler::~cSocketRTScheduler()
{
}

void cSocketRTScheduler::startRun()
{
    if (initsocketlibonce()!=0)
        throw new cRuntimeError("cSocketRTScheduler: Cannot initialize socket library");

    gettimeofday(&baseTime, NULL);

    module = NULL;
    notificationMsg = NULL;
    recvBuffer = NULL;
    recvBufferSize = 0;
    numBytesPtr = NULL;

    port = ev.config()->getAsInt("General", "socketrtscheduler-port", 4242);
    setupListener();
}

void cSocketRTScheduler::setupListener()
{
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket==INVALID_SOCKET)
        throw new cRuntimeError("cSocketRTScheduler: cannot create socket");

    sockaddr_in sinInterface;
    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = INADDR_ANY;
    sinInterface.sin_port = htons(port);
    if (bind(listenerSocket, (sockaddr*)&sinInterface, sizeof(sockaddr_in))==SOCKET_ERROR)
        throw new cRuntimeError("cSocketRTScheduler: socket bind() failed");

    listen(listenerSocket, SOMAXCONN);
}

void cSocketRTScheduler::endRun()
{
}

void cSocketRTScheduler::executionResumed()
{
    gettimeofday(&baseTime, NULL);
    baseTime = timeval_substract(baseTime, sim->simTime());
}

void cSocketRTScheduler::setInterfaceModule(cModule *mod, cMessage *notifMsg, char *buf, int bufSize, int *nBytesPtr)
{
    if (module)
        throw new cRuntimeError("cSocketRTScheduler: setInterfaceModule() already called");
    if (!mod || !notifMsg || !buf || !bufSize || !nBytesPtr)
        throw new cRuntimeError("cSocketRTScheduler: setInterfaceModule(): arguments must be non-NULL");

    module = mod;
    notificationMsg = notifMsg;
    recvBuffer = buf;
    recvBufferSize = bufSize;
    numBytesPtr = nBytesPtr;
    *numBytesPtr = 0;
}

bool cSocketRTScheduler::receiveWithTimeout(long usec)
{
    // prepare sets for select()
    fd_set readFDs, writeFDs, exceptFDs;
    FD_ZERO(&readFDs);
    FD_ZERO(&writeFDs);
    FD_ZERO(&exceptFDs);

    // if we're connected, watch connSocket, otherwise accept new connections
    if (connSocket!=INVALID_SOCKET)
        FD_SET(connSocket, &readFDs);
    else
        FD_SET(listenerSocket, &readFDs);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = usec;

    if (select(FD_SETSIZE, &readFDs, &writeFDs, &exceptFDs, &timeout) > 0)
    {
        // Something happened on one of the sockets -- handle them
        if (connSocket!=INVALID_SOCKET && FD_ISSET(connSocket, &readFDs))
        {
            // receive from connSocket
            char *bufPtr = recvBuffer + (*numBytesPtr);
            int bufLeft = recvBufferSize - (*numBytesPtr);
            if (bufLeft<=0)
                throw new cRuntimeError("cSocketRTScheduler: interface module's recvBuffer is full");
            int nBytes = recv(connSocket, bufPtr, bufLeft, 0);
            if (nBytes==SOCKET_ERROR)
            {
                ev << "cSocketRTScheduler: socket error " << sock_errno() << "\n";
                closesocket(connSocket);
                connSocket = INVALID_SOCKET;
            }
            else if (nBytes == 0)
            {
                ev << "cSocketRTScheduler: socket closed by the client\n";
                if (shutdown(connSocket, SHUT_WR) == SOCKET_ERROR)
                    throw new cRuntimeError("cSocketRTScheduler: shutdown() failed");
                closesocket(connSocket);
                connSocket = INVALID_SOCKET;
            }
            else
            {
                // schedule notificationMsg for the interface module
                ev << "cSocketRTScheduler: received " << nBytes << " bytes\n";
                (*numBytesPtr) += nBytes;

                timeval curTime;
                gettimeofday(&curTime, NULL);
                curTime = timeval_substract(curTime, baseTime);
                simtime_t t = curTime.tv_sec + curTime.tv_usec*1e-6;
                // TBD assert that it's somehow not smaller than previous event's time
                notificationMsg->setArrival(module,-1,t);
                simulation.msgQueue.insert(notificationMsg);
                return true;
            }
        }
        else if (FD_ISSET(listenerSocket, &readFDs))
        {
            // accept connection, and store FD in connSocket
            sockaddr_in sinRemote;
            int addrSize = sizeof(sinRemote);
            connSocket = accept(listenerSocket, (sockaddr*)&sinRemote, (socklen_t*)&addrSize);
            if (connSocket==INVALID_SOCKET)
                throw new cRuntimeError("cSocketRTScheduler: accept() failed");
            ev << "cSocketRTScheduler: connected!\n";
        }
    }
    return false;
}

int cSocketRTScheduler::receiveUntil(const timeval& targetTime)
{
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking ev.idle()
    timeval curTime;
    gettimeofday(&curTime, NULL);
    while (targetTime.tv_sec-curTime.tv_sec >=2 ||
           timeval_diff_usec(targetTime, curTime) >= 200000)
    {
        if (receiveWithTimeout(100000)) // 100ms
            return 1;
        if (ev.idle())
            return -1;
        gettimeofday(&curTime, NULL);
    }

    // difference is now at most 100ms, do it at once
    long usec = timeval_diff_usec(targetTime, curTime);
    if (usec>0)
        if (receiveWithTimeout(usec))
            return 1;
    return 0;
}

cMessage *cSocketRTScheduler::getNextEvent()
{
    // assert that we've been configured
    if (!module)
        throw new cRuntimeError("cSocketRTScheduler: setInterfaceModule() not called: it must be called from a module's initialize() function");

    // calculate target time
    timeval targetTime;
    cMessage *msg = sim->msgQueue.peekFirst();
    if (!msg)
    {
        // if there are no events, wait until something comes from outside
        // TBD: obey simtimelimit, cpu-time-limit
        targetTime.tv_sec = LONG_MAX;
        targetTime.tv_usec = 0;
    }
    else
    {
        // use time of next event
        simtime_t eventSimtime = msg->arrivalTime();
        targetTime = timeval_add(baseTime, eventSimtime);
    }

    // if needed, wait until that time arrives
    timeval curTime;
    gettimeofday(&curTime, NULL);
    if (timeval_greater(targetTime, curTime))
    {
        int status = receiveUntil(targetTime);
        if (status == -1)
            return NULL; // interrupted by user
        if (status == 1)
            msg = sim->msgQueue.peekFirst(); // received something
    }
    else
    {
        // we're behind -- customized versions of this class may
        // alert if we're too much behind, whatever that means
    }

    // ok, return the message
    return msg;
}

void cSocketRTScheduler::sendBytes(const char *buf, size_t numBytes)
{
    if (connSocket==INVALID_SOCKET)
        throw new cRuntimeError("cSocketRTScheduler: sendBytes(): no connection");

    send(connSocket, buf, numBytes, 0);
    // TBD check for errors
}

