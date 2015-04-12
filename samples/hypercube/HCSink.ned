//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Destroys packets and creates statistics
//
simple HCSink
{
    parameters:
        int address;
        @display("i=block/sink");
        @signal[endToEndDelay](type="simtime_t");
        @signal[hopCount](type="long");
        @signal[hopRatio](type="double");
        @statistic[endToEndDelay](title="end-to-end delay";unit=s;record=vector,mean,max;interpolationmode=none);
        @statistic[hopCount](title="hop count";record=vector?,mean,max;interpolationmode=none);
        @statistic[hopRatio](title="hop count/shortest path hops";record=mean,max;interpolationmode=none);
    gates:
        input in;
}


