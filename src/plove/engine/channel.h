//=========================================================================
//  CHANNEL.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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
        // (e.g. in VectorFileReader which uses std::vector). Node ptrs are OK.
        std::deque<Datum> buffer;
        Node *producernode;
        Node *consumernode;
        bool producerfinished;
        bool consumerfinished;
    public:
        Channel();
        ~Channel() {}

        Node *producerNode() const {return producernode;}
        void setProducerNode(Node *node) {producernode = node;}

        Node *consumerNode() const {return consumernode;}
        void setConsumerNode(Node *node) {consumernode = node;}

        /**
         * Returns ptr to the first buffered data item (next one to be read), or NULL
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
         * there won't be any more data except those already in the buffer
         */
        bool closing()  {return producerfinished;}

        /**
         * Returns true if close() has been called and there's no buffered data
         */
        bool eof()  {return producerfinished && length()==0;}

        /**
         * Called by the producer to declare it will not write any more --
         * if also there's no more buffered data (length()==0), that means EOF.
         */
        void close()  {producerfinished=true;}

        /**
         * Called when consumer has finished. Causes channel to ignore
         * further writes (discard any data written).
         */
        void consumerClose() {buffer.clear();consumerfinished=true;}

        /**
         * Returns true when the consumer has closed the channel, that is, 
         * it will not read any more data from the channel.
         */
        bool consumerClosed() {return consumerfinished;}

        /**
         * Number of currently buffered items.
         */
        int length() {return buffer.size();}
};

#endif


