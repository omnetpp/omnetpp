//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import org.omnetpp.queueing.Classifier;
import org.omnetpp.queueing.Delay;
import org.omnetpp.queueing.PassiveQueue;
import org.omnetpp.queueing.Server;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;


//
// The model demonstrates a queueing network. While the model is an abstract
// one, the following more-or-less real-life analogy can help make sense
// of it:
//
//   Passengers arrive at the airport building at random intervals, and they want
//   to check in at a particular counter. They first go to the information desk.
//   There it turns out that some passengers should have gone to a different
//   terminal, so they leave the building. Others have to walk along a long
//   corridor to get to the check-in, where there're different queues for economy
//   class, business class, first class etc. Unfortunately there's only one
//   check-in counter open. It serves the queues in priority order -- economy class
//   passengers are only served if there're no business class or first class
//   passengers. After check-in, passengers get into the duty-free zone and
//   spend their time there until boarding.
//
// In the model, the "enter" module generates messages with different message
// types (message kinds 0,1,2 and 3 with uniform probability). These messages
// get into "classifier" which looks at the message kind and forwards the
// message on the corresponding gate (kind=0 on gate "out[0]", kind=1 on gate
// "out[1]", kind=2 on gate "out[2]", and the rest (kind=3) on the gate
// "rest". The latter messages get discarded in the "toOtherTerminal" module,
// the others get into "delay" modules where they each suffer a small constant
// delay. After the delay they are queued up in "passiveq" passive queues,
// served by "qserver".
//
// "qserver" serves the three queues in priority order: the "businessClass"
// queue is only examined when the "firstClass" queue is empty, and
// "economyClass" queue is only examined when both "businessClass" and
// "firstClass" are empty. Examination of the queues is programmed by calling
// a method in the passive queue module objects (as opposed to message
// exchange, or integrating all queues and the server into a single module).
// Once the "checkin" queue server decided from which queue to obtain a message, it
// tells the passive queue object to send it (this is also done via a method call.)
// The processing time in the server is random. After processing, messages are
// forwarded into the "boarding" module where they end their life circles.
//
// In "boarding", statistics are collected about the time messages spent in
// the system. These statistics can be viewed during simulation (open the
// inspector window for "boarding", then click the "Contents" tab and open
// inspectors for the statistics objects), or after simulation (open
// omnetpp.vec in the IDE).
//
network Terminal
{
    parameters:
        @display("i=block/users");
    submodules:
        arrival: Source {
            @display("p=32.0,132.0;i=block/arrival");
        }
        classifier: Classifier {
            @display("p=124.0,132.0");
        }
        toOtherTerminal: Sink {
            @display("p=124.0,225.0");
        }
        delay1: Delay {
            @display("p=222.0,39.0");
        }
        delay2: Delay {
            @display("p=222.0,132.0");
        }
        delay3: Delay {
            @display("p=222.0,225.0");
        }
        firstClass: PassiveQueue {
            @display("p=329.0,39.0");
        }
        businessClass: PassiveQueue {
            @display("p=329.0,132.0");
        }
        economyClass: PassiveQueue {
            @display("p=329.0,225.0");
        }
        checkin: Server {
            @display("p=448.0,132.0");
        }
        boarding: Sink {
            @display("p=546.0,132.0;i=block/departure");
        }
    connections:
        arrival.out --> classifier.in++;
        classifier.out++ --> delay1.in++;
        classifier.out++ --> delay2.in++;
        classifier.out++ --> delay3.in++;
        firstClass.out++ --> checkin.in++;
        businessClass.out++ --> checkin.in++;
        economyClass.out++ --> checkin.in++;
        checkin.out --> boarding.in++;
        delay1.out --> firstClass.in++;
        delay2.out --> businessClass.in++;
        delay3.out --> economyClass.in++;
        classifier.rest --> toOtherTerminal.in++;
}
