//=========================================================================
//
// CPARSIMCOMM.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2001 Eric Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __CPARSIMCOMM_H__
#define __CPARSIMCOMM_H__

// forward declaration:
class cCommBuffer;


/**
 * Basic communications layer for parallel simulation. It provides an
 * abstraction layer above MPI, PVM, shared-memory communications, etc.
 *
 * This is an interface: functionality is added by specific subclasses
 * for MPI, PVM, etc.
 *
 * @ingroup Parsim
 */
class cParsimCommunications
{
  public:
    /**
     * Virtual destructor.
     */
    virtual ~cParsimCommunications() {}

    /** @name Lifecycle of communications library */
    //@{
    /**
     * Init the library -- parameters (if there are any) should have
     * been passed in constructor.
     *
     * This process may also include starting the program on all
     * processors needed for the current simulation run, if
     * it is not handled by the parallel runtime system (e.g. MPI
     * handles it via mpirun).
     */
    virtual void init() = 0;

    /**
     * Shutdown the communications library.
     */
    virtual void shutdown() = 0;
    //@}

    //@{
    /**
     * Returns total number of partitions.
     */
    virtual int getNumPartitions() = 0;

    /**
     * Returns id of this partition, an integer in the range 0..getNumPartitions()-1.
     */
    virtual int getProcId() = 0;

    /** @name Buffers, send, receive */
    //@{
    /**
     * Creates an empty buffer that can be used to send/receive data
     */
    virtual cCommBuffer *createCommBuffer() = 0;

    /**
     * Recycle communication buffer after use. May be used to implement pooling, etc.
     */
    virtual void recycleCommBuffer(cCommBuffer *buffer) = 0;

    /**
     * Sends packed data with given tag to destination.
     */
    virtual void send(cCommBuffer *buffer, int tag, int destination) = 0;

    /**
     * Sends packed data with given tag to all partitions.
     */
    virtual void broadcast(cCommBuffer *buffer, int tag) = 0;

    /**
     * Receives packed data with given tag from given destination.
     * Normally returns true; false is returned if blocking was interrupted by the user.
     */
    virtual bool receiveBlocking(cCommBuffer *buffer, int& receivedTag, int& sourceProcId) = 0;

    /**
     * Receives packed data with given tag from given destination.
     * Call is non-blocking -- it returns true if something has been
     * received, false otherwise.
     */
    virtual bool receiveNonblocking(cCommBuffer *buffer, int& receivedTag, int& sourceProcId) = 0;

    /**
     * Blocks until all partitions call the same method.
     */
    virtual void synchronize() = 0;
    //@}
};

#endif

