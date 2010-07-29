//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Module for parsim testing.
//
simple Tic
{
    parameters:
        bool initialSend = default(true);  // whether to send a packet in initialize()
        string outputGate = default("g$o");  // on which gate to send
        bool delete = default(true);  // whether to delete incoming packets and send back new ones
        bool allowPointerAliasing = default(false); // whether new message may be at the same address as incoming deleted one
    gates:
        // may be connected in several ways, for testing purposes
        input in @loose;
        output out @loose;
        inout g @loose;
        input inv[] @loose;
        output outv[] @loose;
        inout gv[] @loose;
}

