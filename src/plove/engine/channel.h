//=========================================================================
//  CHANNEL.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "util.h"
#include <deque>
#include "node.h"


/**
 * Does buffering between two processing nodes (Node).
 *
 * @see Node, Port, Datum
 */
class Channel
{
    private:
        // note: a Channel should *never* hold a pointer back to its Ports
        // because ports may be copied after having been assigned to channels
        // (e.g. in VectorFileReader which uses std::vector)
        bool nomorewrites;
        std::deque<Datum> buffer;
        Node *consumernode;
    public:
        Channel() {nomorewrites=false;}
        ~Channel() {}

        Node *consumerNode() const {return consumernode;}
        void setConsumerNode(Node *node) {consumernode = node;}

        const Datum *peek() const;
        void write(Datum *a, int n);
        int read(Datum *a, int max);
        void flush() {buffer.clear();}
        bool closing()  {return nomorewrites;}
        bool eof()  {return nomorewrites && length()==0;}
        void close()  {nomorewrites=true;}

        /** Number of buffered items */
        int length() {return buffer.size();}
};

#endif


