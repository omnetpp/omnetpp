//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "ppsource",
    "ppsink",
    "ppif";



//
// Transmission of packets on a line with finite bandwidth. Uses a pair of
// point-to-point interfaces. The line is driven by bursty traffic.
//
module PointToPointNet
    submodules:
        burstySrc: PPSource;
            display: "p=82,92;i=block/source";
        ppIF1: PointToPointIF;
            display: "p=170,92;i=block/activeq;q=queue";
        ppIF2: PointToPointIF;
            display: "p=336,92;i=block/activeq;q=queue";
        sink: PPSink;
            display: "p=430,92;i=block/sink";
    connections nocheck:
        burstySrc.out --> ppIF1.in;
        ppIF1.lineOut --> datarate 1000000 --> ppIF2.lineIn display "o=,3";
        ppIF1.lineIn <-- datarate 1000000 <-- ppIF2.lineOut display "o=,3";
        ppIF2.out --> sink.in;
endmodule

network pointToPointNet : PointToPointNet
endnetwork

