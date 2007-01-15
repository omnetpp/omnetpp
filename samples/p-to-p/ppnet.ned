//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "ppsource";
import "ppsink";
import "ppif";



//
// Transmission of packets on a line with finite bandwidth. Uses a pair of
// point-to-point interfaces. The line is driven by bursty traffic.
//
network PointToPointNet
{
    types:
        channel C extends cBasicChannel
        {
            parameters:
                datarate = 1Mbps;
                delay = 10ms;
                @display("o=,3");
        }
    submodules:
        burstySrc: PPSource {
            parameters:
                @display("p=82,92");
        };
        ppIF1: PointToPointIF {
            parameters:
                @display("p=170,92");
        };
        ppIF2: PointToPointIF {
            parameters:
                @display("p=336,92");
        };
        sink: PPSink {
            parameters:
                @display("p=430,92");
        };
    connections allowunconnected:
        burstySrc.out --> ppIF1.in;
        ppIF2.out --> sink.in;

        ppIF1.line <--> C <--> ppIF2.line;
}

