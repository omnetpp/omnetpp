//=========================================================================
//  CSOCKETRTSCHEDULER.H - part of
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

#ifndef __CSOCKETRTSCHEDULER_H__
#define __CSOCKETRTSCHEDULER_H__

#include "platdep/time.h"
#include "platdep/sockets.h"
#include <omnetpp.h>


/**
 * Real-time scheduler with socket-based external communication.
 *
 * \code
 * class MyInterfaceModule : public cSimpleModule
 * {
 *    cSocketRTScheduler *rtScheduler;
 *    cMessage *extEvent;
 *    char buf[4000];
 *    int numBytes;
 *    ...
 * \endcode
 *
 * \code
 * void MyInterfaceModule::initialize()
 * {
 *     extEvent = new cMessage("extEvent");
 *     rtScheduler = check_and_cast<cSocketRTScheduler *>(simulation.scheduler());
 *     rtScheduler->setInterfaceModule(this, extEvent, buf, 4000, numBytes);
 * }
 * \endcode
 *
 * THIS CLASS IS JUST AN EXAMPLE -- IF YOU WANT TO DO HARDWARE-IN-THE-LOOP
 * SIMULATION, YOU WILL NEED TO WRITE YOUR OWN SCHEDULER WHICH FITS YOUR NEEDS.
 * For example, you'll probably want a different external interface than
 * a single TCP socket: maybe UDP socket, maybe raw socket to grab full Ethernet
 * frames, maybe pipe, maybe USB or other interface, etc.
 */
class cSocketRTScheduler : public cScheduler
{
  protected:
    // config
    int port;

    cModule *module;
    cMessage *notificationMsg;
    char *recvBuffer;
    int recvBufferSize;
    int *numBytesPtr;

    // state
    timeval baseTime;
    SOCKET listenerSocket;
    SOCKET connSocket;

    virtual void setupListener();
    virtual bool receiveWithTimeout(long usec);
    virtual int receiveUntil(const timeval& targetTime);

  public:
    /**
     * Constructor.
     */
    cSocketRTScheduler();

    /**
     * Destructor.
     */
    virtual ~cSocketRTScheduler();

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun();

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun();

    /**
     * Recalculates "base time" from current wall clock time.
     */
    virtual void executionResumed();

    /**
     * To be called from the module which wishes to receive data from the
     * socket. The method must be called from the module's initialize()
     * function.
     */
    virtual void setInterfaceModule(cModule *module, cMessage *notificationMsg,
                                    char *recvBuffer, int recvBufferSize, int *numBytesPtr);

    /**
     * Scheduler function -- it comes from cScheduler interface.
     */
    virtual cMessage *getNextEvent();

    /**
     * Send on the currently open connection
     */
    virtual void sendBytes(const char *buf, size_t numBytes);
};

#endif

