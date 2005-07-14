//=========================================================================
//  DATAFLOWMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _DATAFLOWMANAGER_H_
#define _DATAFLOWMANAGER_H_

#include <vector>
#include "node.h"
#include "channel.h"


/**
 * Controls execution of the data flow network.
 *
 * @see Node, Channel
 */
class DataflowManager
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
        // its input an output channels (side effect!)
        bool nodeFinished(Node *node);

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
         * ready (isReady() method) until they are all done (finished()
         * method). If none of them are ready but there are ones which
         * haven't finished yet, the method declares a deadlock.
         */
        void execute();
};

#endif


