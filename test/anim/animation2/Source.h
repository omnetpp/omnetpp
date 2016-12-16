//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __ANIMATION2_SOURCE_H
#define __ANIMATION2_SOURCE_H

#include <omnetpp.h>

using namespace omnetpp;

namespace animation2 {

class Source : public cSimpleModule
{
  private:
    cMessage *textMessage = nullptr;
    cMessage *operationMessage = nullptr;

    const int numOperations = 7;
    int operation = 0;
    cTextFigure *text;

    std::string title =
        "This test runs a series of operations in a cycle to let you examine\n"
        "the built-in animation behaviour of the runtime environment.\n"
        "All operations are performed by the source in the middle.\n"
        "The nodes next to it react to them in various ways.\n"
        "The surrounding sinks ignore all messages, their methods are empty.\n"
        "The blue channel has delay, the green one has limited data rate, cyan ones have both.\n"
        "All messages are 4096 bytes long packets.";

    std::vector<std::string> explanations = {
        "source sends a message (jobA) to nodeA.\n"
        "In the corresponding handleMessage():\n"
        "nodeA will send a message to sinkAB then to sinkAA (jobB and jobA respectively)\n"
        "However, since there is a delay towards sinkAB, the message sent later (jobA) should\n"
        "be animated (while the simulation time is paused with a hold) and delivered first.",

        "source sends a message (jobB) to nodeB.\n"
        "In the corresponding handleMessage():\n"
        "nodeB will send a message to sinkBB then to sinkBA (jobB and jobA respectively)\n"
        "Data rate is limited on both channels, so the messages should be displayed as lines.\n"
        "Towards sinkBB, there is a delay too. So jobA will be delivered first, and it should be\n"
        "displayed on the whole length of the connection exactly while jobB is being transmitted.\n",

        "source does, in a single event, and in this order:\n"
        "Send jobA to sinkA, call bar() on sinkA, send jobB to sinkB, call bar() on sinkB.\n"
        "As a result, first jobA should be animated to the edge of sinkA, then stopped and kept there.\n"
        "Then the first bar() call should be animated, then the sending, but not the delivery of jobB.\n"
        "Then the animation of the second bar() call, with both messages still at the module edges.\n"
        "Finally jobA and jobB will be delivered in order, and should be animated accordingly.",

        "source calls foo() on nodeA.\n"
        "In the method, nodeA calls bar() first on sinkAA, then on sinkAB.\n"
        "These two subcalls should be animated one after the other, but both while the foo() call\n"
        "on nodeA is still visible, intuitively visualizing a depth-first traversal of a tree.\n",

        "source calls foo() on nodeB.\n"
        "In the method, nodeB calls bar() first on sinkBA, then on sinkBB.\n"
        "These two subcalls should be animated one after the other, but both while the foo() call\n"
        "on nodeB is still visible, intuitively visualizing a depth-first traversal of a tree.\n"
        "(This is basically the same as the previous operation, included just for symmetry's sake.)",

        "source calls baz() on nodeA.\n"
        "In the method, nodeA will call baz() on sinkAA, then send a message (dummyA) to it,\n"
        "then call baz() on sinkAB, and finally send a message (dummyB) to it too.\n"
        "The first call to baz on nodeA should be visible during the subcall towards sinkAA,\n"
        "the animation of sending (but not the delivery) of dummyA, and the subcall towards sinkAB.\n"
        "Then comes the delivery of dummyA, and lastly the sending (with progressing SimTime)\n"
        "and delivery of dummyB. All of dummyB is animated after the original method animation to\n"
        "nodeA ended, because the channel it is sent on has nonzero propagation delay.",

        "source calls baz() on nodeB.\n"
        "In the method, nodeB will call baz() on sinkBA, then send a message (dummyA) to it,\n"
        "then call baz() on sinkBB, and finally send a message (dummyB) to it too.\n"
        "The first call to baz on nodeB should be visible during the subcalls to the sinks.\n"
        "In this case the sending animation of both messages is done after the methodcalls\n"
        "are over, as none of them are instantaneous due to the data rate limit and/or delay\n"
        "of the channels they are sent on."
    };

  public:
     virtual ~Source();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

}; // namespace

#endif
