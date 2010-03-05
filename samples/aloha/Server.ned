//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// The central computer in the ALOHAnet network.
//
simple Server
{
    parameters:
        @display("i=device/antennatower_l");
        @statistic[beginRx](title="begin reception";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[channelUtilization](title="channel utilization";record=vector,last;interpolationmode=linear);
    gates:
        input in @directIn;
}

