//=========================================================================
//  CSOCKETRTSCHEDULER.H - part of
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

#ifndef __CSOCKETRTSCHEDULER_H__
#define __CSOCKETRTSCHEDULER_H__

#include <omnetpp/platdep/sockets.h>
#include <omnetpp.h>

using namespace omnetpp;

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
 *     rtScheduler = check_and_cast<cSocketRTScheduler *>(simulation.getScheduler());
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
    int64_t baseTime; // in microseconds, as returned by opp_get_monotonic_clock_usecs()
    SOCKET listenerSocket = INVALID_SOCKET;
    SOCKET connSocket = INVALID_SOCKET;

    virtual void setupListener();
    virtual bool receiveWithTimeout(long usec);
    virtual int receiveUntil(int64_t targetTime);

  public:
    /**
     * Return a description for the GUI.
     */
    virtual std::string str() const override;

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() override;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() override;

    /**
     * Recalculates "base time" from current wall clock time.
     */
    virtual void executionResumed() override;

    /**
     * To be called from the module which wishes to receive data from the
     * socket. The method must be called from the module's initialize()
     * function.
     */
    virtual void setInterfaceModule(cModule *module, cMessage *notificationMsg,
            char *recvBuffer, int recvBufferSize, int *numBytesPtr);

    /**
     * Returns the first event in the Future Event Set.
     */
    virtual cEvent *guessNextEvent() override;

    /**
     * Scheduler function -- it comes from the cScheduler interface.
     */
    virtual cEvent *takeNextEvent() override;

    /**
     * Undo takeNextEvent() -- it comes from the cScheduler interface.
     */
    virtual void putBackEvent(cEvent *event) override;

    /**
     * Send on the currently open connection
     */
    virtual void sendBytes(const char *buf, size_t numBytes);
};

#endif

