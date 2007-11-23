//=========================================================================
//  NODE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _NODE_H_
#define _NODE_H_

#include "scavedefs.h"
#include "commonutil.h"


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
    BigDecimal xp;  // precise x value, BigDecimal::Nil if not filled in  FIXME REVISE ALL FILTERS WHETHER THEY USE xp CORRECTLY! Andras
    double y;  ///< usually the value at t
    long eventNumber;

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
        Port(Node *owner) {ownernode = owner; chan = 0;}
        Port(const Port& p) {ownernode = p.ownernode; chan = p.chan;}
        ~Port() {}
        void setChannel(Channel *channel) {Assert(!chan); chan = channel;}
        Node *node() {return ownernode;}
        Channel *channel() const  {return chan;}
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
        NodeType *nodetype;
        bool alreadyfinished;

    protected:
        /**
         * Called when the node is added to the dataflow manager.
         */
        void setDataflowManager(DataflowManager *m) {mgr = m;}

    public:
        /**
         * Constructor
         */
        Node() {mgr=NULL; nodetype=NULL; alreadyfinished=false;}

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
        void setNodeType(const NodeType *t)  {nodetype = const_cast<NodeType *>(t);}

        /**
         * Returns the corresponding NodeType class.
         */
        NodeType *nodeType() const {return nodetype;}

        /** Execution and scheduling */
        //@{
        /**
         * Do some amount of work, then return
         */
        virtual void process() = 0;

        /**
         * Are more invocations of process() necessary()?
         */
        virtual bool finished() const = 0;

        /**
         * Provided it has not finished() yet, can process() be invoked
         * right now? (finished() is called first -- isReady() is only
         * invoked if finished() returns false. So isReady() doesn't need
         * to check for eof.)
         */
        virtual bool isReady() const = 0;
        //@}

        /**
         * Invoked by the dataflow manager when the node's finished() first
         * returns true. It sets a status flag so that further invocations
         * of finished() can be avoided.
         */
        void setAlreadyFinished() {alreadyfinished = true;}

        /**
         * Used by the dataflow manager. Returns true if setAlreadyFinished()
         * has already been invoked.
         */
        bool alreadyFinished() {return alreadyfinished;}
};

#endif


