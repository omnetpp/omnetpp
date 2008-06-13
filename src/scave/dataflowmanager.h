//=========================================================================
//  DATAFLOWMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _DATAFLOWMANAGER_H_
#define _DATAFLOWMANAGER_H_

#include <vector>
#include "node.h"
#include "progressmonitor.h"
#include "channel.h"

NAMESPACE_BEGIN


/**
 * Controls execution of the data flow network.
 *
 * @see Node, Channel
 */
class SCAVE_API DataflowManager
{
    protected:
        std::vector<Node *> nodes;
        std::vector<Channel *> channels;
        int threshold; // channel buffer upper limit
        int lastnode; // for round robin

    protected:
        // utility called from connect()
        void addChannel(Channel *channel);

        // scheduler function called by execute()
        Node *selectNode();

        // returns true of a node has finished; if so, also closes
        // its input an output channels.
        bool updateNodeFinished(Node *node);

        // returns true if the node is a reader node
        // (i.e. the category of its type is "reader-node")
        bool isReaderNode(Node *node);

        // helper to estimate the total amount of work
        int64 totalBytesToBeRead();

    public:
        /**
         * Constructor
         */
        DataflowManager();

        /**
         * Destructor
         */
        ~DataflowManager();

        /**
         * Add a node to the data-flow network.
         */
        void addNode(Node *node);

        /**
         * Connects two Node ports via a Channel object.
         */
        void connect(Port *src, Port *dest);

        /**
         * Executes the data-flow network. That will basically keep
         * calling the process() method of nodes that say they are
         * ready (isReady() method) until they are all done (isFinished()
         * method). If none of them are ready but there are ones which
         * haven't finished yet, the method declares a deadlock.
         */
        void execute(IProgressMonitor *monitor = NULL);

        /**
         * For debugging: dumps the data-flow network on the standard output.
         */
        void dump();
};

NAMESPACE_END


#endif


