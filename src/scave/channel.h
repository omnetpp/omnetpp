//=========================================================================
//  CHANNEL.H - part of
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

#ifndef __OMNETPP_SCAVE_CHANNEL_H
#define __OMNETPP_SCAVE_CHANNEL_H

#include <deque>
#include "common/commonutil.h"
#include "node.h"

namespace omnetpp {
namespace scave {


/**
 * Does buffering between two processing nodes (Node).
 *
 * @see Node, Port, Datum
 */
class SCAVE_API Channel
{
    private:
        // note: a Channel should *never* hold a pointer back to its Ports
        // because ports may be copied after having been assigned to channels
        // (e.g. in VectorFileReader which uses std::vector). Node ptrs are OK.
        std::deque<Datum> buffer;  //XXX deque has very poor performance under VC++ (pagesize==1!), consider using std::vector here instead
        Node *producerNode;
        Node *consumerNode;
        bool producerFinished;
        bool consumerFinished;
    public:
        Channel();
        ~Channel() {}

        void setProducerNode(Node *node) {producerNode = node;}
        Node *getProducerNode() const {return producerNode;}

        void setConsumerNode(Node *node) {consumerNode = node;}
        Node *getConsumerNode() const {return consumerNode;}

        /**
         * Returns ptr to the first buffered data item (next one to be read), or nullptr
         */
        const Datum *peek() const;

        /**
         * Writes an array.
         */
        void write(Datum *a, int n);

        /**
         * Reads into an array. Returns number of items actually stored.
         */
        int read(Datum *a, int max);

        /**
         * Returns true if producer has already called close() which means
         * there will not be any more data except those already in the buffer
         */
        bool isClosing()  {return producerFinished;}

        /**
         * Returns true if close() has been called and there is no buffered data
         */
        bool eof()  {return producerFinished && length()==0;}

        /**
         * Called by the producer to declare it will not write any more --
         * if also there is no more buffered data (length()==0), that means EOF.
         */
        void close()  {producerFinished=true;}

        /**
         * Called when consumer has finished. Causes channel to ignore
         * further writes (discard any data written).
         */
        void consumerClose() {buffer.clear();consumerFinished=true;}

        /**
         * Returns true when the consumer has closed the channel, that is,
         * it will not read any more data from the channel.
         */
        bool isConsumerClosed() {return consumerFinished;}

        /**
         * Number of currently buffered items.
         */
        int length() {return buffer.size();}
};

} // namespace scave
}  // namespace omnetpp


#endif


