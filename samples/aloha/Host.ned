//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A computer in the ALOHAnet network.
//
simple Host
{
    parameters:
        double txRate @unit(bps);  // transmission rate
        double radioDelay;  // propagation delay of radio link
        volatile int pkLenBits;  // packet length in bits
        volatile double iaTime;  // packet interarrival time
        double slotTime;  // zero means no slots (pure Aloha)
        @display("i=device/pc_s");
}

