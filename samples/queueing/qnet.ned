//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import "classifier.ned";
import "delay.ned";
import "source.ned";
import "pqueue.ned";
import "server.ned";
import "sink.ned";

module QNet
{
    submodules:
        enter: QSource {
            parameters:
                @display("p=63,130");
        };
        classifier: QClassifier {
            parameters:
                @display("p=160,130");
            gates:
                out[3];
        };
        delay_[3]: QDelay {
            parameters:
                @display("p=263,70,column");
        };
        passiveq[3]: QPassiveQueue {
            parameters:
                @display("p=349,70,column");
            gates:
                out[1];
        };
        qserver: QServer {
            parameters:
                @display("p=443,130");
            gates:
                in[3];
        };
        leave2: QSink {
            parameters:
                @display("p=546,130");
        };
        leave1: QSink {
            parameters:
                @display("p=160,200");
        };
    connections:
        enter.out --> classifier.in;
        for i=0..2 {
            classifier.out[i] --> delay_[i].in;
            delay_[i].out --> passiveq[i].in;
            passiveq[i].out[0] --> qserver.in[i];
        };
        qserver.out --> leave2.in;
        classifier.other --> leave1.in;
}

network qnet extends QNet
{
}
