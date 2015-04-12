//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

network FifoNet1
{
    parameters:
        volatile double serviceRate1 = default(220);
        volatile double serviceRate2 = default(220);
    submodules:
        source1: FF1Generator {
            parameters:
                messageKind = 1;
                arrivalRate = 100;
                @display("p=50,53;i=gen");
        };
        source2: FF1Generator {
            parameters:
                messageKind = 2;
                arrivalRate = 100;
                @display("p=50,113;i=gen");
        };
        queue1: FF1PacketFifo {
            parameters:
                serviceRate = serviceRate1;
                @display("p=165,80;i=queue;q=queue");
            gates:
                in[2];
        };
        queue2: FF1PacketFifo {
            parameters:
                serviceRate = serviceRate2;
                @display("p=258,80;i=queue;q=queue");
            gates:
                in[1];
        };
        sink: FF1Sink {
            parameters:
                @display("p=345,80;i=sink");
        };
    connections:
        source1.out --> queue1.in[0];
        source2.out --> queue1.in[1];
        queue1.out --> queue2.in[0];
        queue2.out --> sink.in;
}

