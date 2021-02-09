//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
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
    submodules:
        enter: QSource;
            parameters:
            display: "p=63,130;i=block/source";
        classifier: QClassifier;
            gatesizes:
                out[3];
            display: "p=160,130;i=block/fork";
        delay_: QDelay[3];
            display: "p=263,70,column;i=block/delay";
        passiveq: QPassiveQueue[3];
            gatesizes:
                out[1];
            display: "p=349,70,column;i=block/process;q=queue";
        qserver: QServer;
            gatesizes:
                in[3];
            display: "p=443,130;i=block/activeq";
        leave2: QSink;
            display: "p=546,130;i=block/sink";
        leave1: QSink;
            display: "p=160,200;i=block/sink";
    connections:
        enter.out --> classifier.in;
        for i=0..2 do
            classifier.out[i] --> delay_[i].in;
            delay_[i].out --> passiveq[i].in;
            passiveq[i].out[0] --> qserver.in[i];
        endfor;
        qserver.out --> leave2.in;
        classifier.other --> leave1.in;
endmodule

network qnet : QNet
endnetwork
