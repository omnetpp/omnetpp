//=========================================================================
//  CSOCKETRTSCHEDULER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005-2015 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "SocketRTScheduler.h"

Register_Class(cSocketRTScheduler);

Register_GlobalConfigOption(CFGID_SOCKETRTSCHEDULER_PORT, "socketrtscheduler-port", CFG_INT, "4242", "When cSocketRTScheduler is selected as scheduler class: the port number cSocketRTScheduler listens on.");

inline std::ostream& operator<<(std::ostream& out, const timeval& tv)
{
    return out << (unsigned long)tv.tv_sec << "s" << tv.tv_usec << "us";
}

//---

std::string cSocketRTScheduler::str() const
{
    return "socket RT scheduler";
}

void cSocketRTScheduler::startRun()
{
    if (initsocketlibonce() != 0)
        throw cRuntimeError("cSocketRTScheduler: Cannot initialize socket library");

    baseTime = opp_get_monotonic_clock_usecs();

    module = nullptr;
    notificationMsg = nullptr;
    recvBuffer = nullptr;
    recvBufferSize = 0;
    numBytesPtr = nullptr;

    port = getEnvir()->getConfig()->getAsInt(CFGID_SOCKETRTSCHEDULER_PORT);
    setupListener();
}

void cSocketRTScheduler::setupListener()
{
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket == INVALID_SOCKET)
        throw cRuntimeError("cSocketRTScheduler: cannot create socket");

    // Setting this option makes it possible to kill the sample and restart
    // it right away without having to change the port it is listening on.
    // Not using SO_REUSEADDR as per: https://stackoverflow.com/a/34812994
    // Correction: There is no SO_REUSEPORT on Windows, so SO_REUSEADDR it is.
    int enable = 1;
    if (setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int)) < 0)
        throw cRuntimeError("cSocketRTScheduler: cannot set socket option");

    sockaddr_in sinInterface;
    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = INADDR_ANY;
    sinInterface.sin_port = htons(port);
    if (bind(listenerSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in)) == SOCKET_ERROR)
        throw cRuntimeError("cSocketRTScheduler: socket bind() failed");

    listen(listenerSocket, SOMAXCONN);
}

void cSocketRTScheduler::endRun()
{
}

void cSocketRTScheduler::executionResumed()
{
    baseTime = opp_get_monotonic_clock_usecs();
    baseTime = baseTime - simTime().inUnit(SIMTIME_US);
}

void cSocketRTScheduler::setInterfaceModule(cModule *mod, cMessage *notifMsg, char *buf, int bufSize, int *nBytesPtr)
{
    if (module)
        throw cRuntimeError("cSocketRTScheduler: setInterfaceModule() already called");
    if (!mod || !notifMsg || !buf || !bufSize || !nBytesPtr)
        throw cRuntimeError("cSocketRTScheduler: setInterfaceModule(): arguments must be non-nullptr");

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
    if (connSocket != INVALID_SOCKET)
        FD_SET(connSocket, &readFDs);
    else
        FD_SET(listenerSocket, &readFDs);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = usec;

    if (select(FD_SETSIZE, &readFDs, &writeFDs, &exceptFDs, &timeout) > 0) {
        // Something happened on one of the sockets -- handle them
        if (connSocket != INVALID_SOCKET && FD_ISSET(connSocket, &readFDs)) {
            // receive from connSocket
            char *bufPtr = recvBuffer + (*numBytesPtr);
            int bufLeft = recvBufferSize - (*numBytesPtr);
            if (bufLeft <= 0)
                throw cRuntimeError("cSocketRTScheduler: interface module's recvBuffer is full");
            int nBytes = recv(connSocket, bufPtr, bufLeft, 0);
            if (nBytes == SOCKET_ERROR) {
                EV << "cSocketRTScheduler: socket error " << sock_errno() << "\n";
                closesocket(connSocket);
                connSocket = INVALID_SOCKET;
            }
            else if (nBytes == 0) {
                EV << "cSocketRTScheduler: socket closed by the client\n";
                if (shutdown(connSocket, SHUT_WR) == SOCKET_ERROR)
                    throw cRuntimeError("cSocketRTScheduler: shutdown() failed");
                closesocket(connSocket);
                connSocket = INVALID_SOCKET;
            }
            else {
                // schedule notificationMsg for the interface module
                EV << "cSocketRTScheduler: received " << nBytes << " bytes\n";
                (*numBytesPtr) += nBytes;

                int64_t currentTime = opp_get_monotonic_clock_usecs();
                simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
                ASSERT(eventTime >= simTime());
                notificationMsg->setArrival(module->getId(), -1, eventTime);
                getSimulation()->getFES()->insert(notificationMsg);
                return true;
            }
        }
        else if (FD_ISSET(listenerSocket, &readFDs)) {
            // accept connection, and store FD in connSocket
            sockaddr_in sinRemote;
            int addrSize = sizeof(sinRemote);
            connSocket = accept(listenerSocket, (sockaddr *)&sinRemote, (socklen_t *)&addrSize);
            if (connSocket == INVALID_SOCKET)
                throw cRuntimeError("cSocketRTScheduler: accept() failed");
            EV << "cSocketRTScheduler: connected!\n";
        }
    }
    return false;
}

int cSocketRTScheduler::receiveUntil(int64_t targetTime)
{
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking getEnvir()->idle()
    int64_t currentTime = opp_get_monotonic_clock_usecs();
    while (targetTime - currentTime >= 200000)
    {
        if (receiveWithTimeout(100000))  // 100ms
            return 1;

        // update simtime before calling envir's idle()
        currentTime = opp_get_monotonic_clock_usecs();
        simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
        ASSERT(eventTime >= simTime());
        sim->setSimTime(eventTime);
        if (getEnvir()->idle())
            return -1;
        currentTime = opp_get_monotonic_clock_usecs();
    }

    // difference is now at most 100ms, do it at once
    long remaining = targetTime - currentTime;
    if (remaining > 0)
        if (receiveWithTimeout(remaining))
            return 1;

    return 0;
}

cEvent *cSocketRTScheduler::guessNextEvent()
{
    return sim->getFES()->peekFirst();
}

cEvent *cSocketRTScheduler::takeNextEvent()
{
    // assert that we've been configured
    if (!module)
        throw cRuntimeError("cSocketRTScheduler: setInterfaceModule() not called: it must be called from a module's initialize() function");

    // calculate target time
    int64_t targetTime;
    cEvent *event = sim->getFES()->peekFirst();
    if (!event) {
        // if there are no events, wait until something comes from outside
        // TBD: obey simtimelimit, cpu-time-limit
        // This way targetTime will always be "as far in the future as possible", considering
        // how integer overflows work in conjunction with comparisons in C++ (in practice...)
        targetTime = opp_get_monotonic_clock_usecs() + INT64_MAX;
    }
    else {
        // use time of next event
        simtime_t eventSimtime = event->getArrivalTime();
        targetTime = baseTime + eventSimtime.inUnit(SIMTIME_US);
    }

    // if needed, wait until that time arrives
    int64_t currentTime = opp_get_monotonic_clock_usecs();
    if (targetTime > currentTime) {
        int status = receiveUntil(targetTime);
        if (status == -1)
            return nullptr;  // interrupted by user
        if (status == 1)
            event = sim->getFES()->peekFirst();  // received something
    }
    else {
        // we're behind -- customized versions of this class may
        // alert if we're too much behind, whatever that means
    }

    // remove event from FES and return it
    cEvent *tmp = sim->getFES()->removeFirst();
    ASSERT(tmp == event);
    return event;
}

void cSocketRTScheduler::putBackEvent(cEvent *event)
{
    sim->getFES()->putBackFirst(event);
}

void cSocketRTScheduler::sendBytes(const char *buf, size_t numBytes)
{
    if (connSocket == INVALID_SOCKET)
        throw cRuntimeError("cSocketRTScheduler: sendBytes(): no connection");

    send(connSocket, buf, numBytes, 0);
    // TBD check for errors
}

