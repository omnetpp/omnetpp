//=========================================================================
//  NODE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_NODE_H
#define __OMNETPP_SCAVE_NODE_H

#include "common/commonutil.h"
#include "scavedefs.h"

namespace omnetpp {
namespace scave {


class Node;
class NodeType;
class Channel;
class DataflowManager;

/**
 * Represents a data element in an output vector file. Processing nodes
 * (Node) process elements of this type.
 *
 * @see Node, Channel, Port
 */
struct Datum
{
    // x value stored as a double or a BigDecimal, usually the simulation time t
    double x;       // x value as double
    BigDecimal xp;  // precise x value, BigDecimal::Nil if not filled in
    double y;       // usually the value at t
    eventnumber_t eventNumber;

    Datum() : xp(BigDecimal::Nil), eventNumber(-1) {}
};


/**
 * Connection point of channels in processing nodes.
 *
 * @see Datum, Node, Channel
 */
class SCAVE_API Port
{
    private:
        Node *ownernode;
        Channel *chan;
    public:
        Port(Node *owner) {ownernode = owner; chan = nullptr;}
        Port(const Port& p) {ownernode = p.ownernode; chan = p.chan;}
        ~Port() {}
        void setChannel(Channel *channel) {Assert(!chan); chan = channel;}
        Node *getNode() {return ownernode;}
        Channel *getChannel() const  {return chan;}
        Channel *operator()() const {Assert(chan); return chan;}
};


/**
 * Processing node. Processing nodes can be connected via ports and channels
 * to form a data flow network.
 *
 * @see DataflowManager, Port, Channel, Datum, NodeType
 */
class SCAVE_API Node
{
        friend class DataflowManager;

    private:
        DataflowManager *mgr;
        NodeType *nodeType;
        bool alreadyFinished;

    protected:
        /**
         * Called when the node is added to the dataflow manager.
         */
        void setDataflowManager(DataflowManager *m) {mgr = m;}

    public:
        /**
         * Constructor
         */
        Node() {mgr=nullptr; nodeType=nullptr; alreadyFinished=false;}

        /**
         * Virtual destructor
         */
        virtual ~Node() {}

        /**
         * Returns the dataflow manager in which this node is inserted.
         */
        DataflowManager *dataflowManager() const {return mgr;}

        /**
         * Should be called right after construction by the corresponding
         * NodeType class.
         */
        void setNodeType(const NodeType *t)  {nodeType = const_cast<NodeType *>(t);}

        /**
         * Returns the corresponding NodeType class.
         */
        NodeType *getNodeType() const {return nodeType;}

        /** @name Execution and scheduling */
        //@{
        /**
         * Do some amount of work, then return
         */
        virtual void process() = 0;

        /**
         * Are more invocations of process() necessary()?
         */
        virtual bool isFinished() const = 0;

        /**
         * Provided it has not isFinished() yet, can process() be invoked
         * right now? (isFinished() is called first -- isReady() is only
         * invoked if isFinished() returns false. So isReady() doesn't need
         * to check for eof.)
         */
        virtual bool isReady() const = 0;
        //@}

        /**
         * Invoked by the dataflow manager when the node's isFinished() first
         * returns true. It sets a status flag so that further invocations
         * of isFinished() can be avoided.
         */
        void setAlreadyFinished() {alreadyFinished = true;}

        /**
         * Used by the dataflow manager. Returns true if setAlreadyFinished()
         * has already been invoked.
         */
        bool getAlreadyFinished() {return alreadyFinished;}
};

} // namespace scave
}  // namespace omnetpp


#endif


