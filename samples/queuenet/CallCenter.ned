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
import org.omnetpp.queueing.Router;
import org.omnetpp.queueing.Server;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;


//
// A call center simulation.
//
network CallCenter
{
    parameters:
        @display("i=device/cellphone");
    submodules:
        incomingCall: Source {
            @display("p=34.0,236.0");
        }
        classifier: Classifier {
            @display("p=130.0,236.0");
        }
        switchingDelay: Delay {
            @display("p=282.0,323.0");
        }
        router: Router {
            @display("p=490.0,236.0");
            gates:
                out[2];
        }
        billing: PassiveQueue {
            @display("p=225.0,27.0");
        }
        marketing: PassiveQueue {
            @display("p=225.0,104.0");
        }
        support: PassiveQueue {
            @display("p=225.0,169.0");
        }
        internal: PassiveQueue {
            @display("p=225.0,236.0");
        }
        operator1: Server {
            @display("p=366.0,26.0");
        }
        operator2: Server {
            @display("p=366.0,104.0");
        }
        operator3: Server {
            @display("p=366.0,169.0");
        }
        operator4: Server {
            @display("p=366.0,236.0");
        }
        callServed: Sink {
            @display("p=559.0,134.0");
        }
        hangingUp: Sink {
            @display("p=131.0,330.0");
        }
    connections:
        incomingCall.out --> classifier.in++;
        classifier.out++ --> billing.in++;
        classifier.out++ --> marketing.in++;
        classifier.out++ --> support.in++;
        classifier.out++ --> internal.in++;
        billing.out++ --> operator1.in++;
        billing.out++ --> operator2.in++;
        marketing.out++ --> operator2.in++;
        support.out++ --> operator2.in++;
        support.out++ --> operator3.in++;
        internal.out++ --> operator4.in++;
        operator1.out --> router.in++;
        operator2.out --> router.in++;
        operator3.out --> router.in++;
        operator4.out --> router.in++;
        router.out[0] --> callServed.in++;
        router.out[1] --> switchingDelay.in++;
        switchingDelay.out --> classifier.in++;
        classifier.rest --> hangingUp.in++;
}
