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
                @display("p=63,130;i=block/source");
        };
        classifier: QClassifier {
            parameters:
                @display("p=160,130;i=block/fork");
            gates:
                out[3];
        };
        delay_[3]: QDelay {
            parameters:
                @display("p=263,70,column;i=block/delay");
        };
        passiveq[3]: QPassiveQueue {
            parameters:
                @display("p=349,70,column;i=block/passiveq;q=queue");
            gates:
                out[1];
        };
        qserver: QServer {
            parameters:
                @display("p=443,130;i=block/server");
            gates:
                in[3];
        };
        leave2: QSink {
            parameters:
                @display("p=546,130;i=block/sink");
        };
        leave1: QSink {
            parameters:
                @display("p=160,200;i=block/sink");
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
