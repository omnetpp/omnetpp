//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1998-2015 Andras Varga
// Copyright (C) 1996-97 Gabor Lencse, T.U.Budapest, Dept. of Telecommunications
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates messages for an FDDI Station.
//
simple FDDI_Generator
    parameters:
        StationID: numeric,
        address: string;
    gates:
        out: out;
endsimple


//
// Generates new addresses in an FDDI Router.
//
simple FDDI_Address_Generator
    parameters:
        StationID: numeric,
        address: string;
    gates:
        in: in;
        out: out;
endsimple


//
// Generates messages.
//
simple FDDI_Generator4Ring
    parameters:
        no_msg: numeric,
        no_comps: numeric const,
        wait_time: numeric;
    gates:
        out: out;
endsimple


//
// Generates messages for an FDDI Sniffer.
//
simple FDDI_Generator4Sniffer
    parameters:
        StationID: numeric,
        address: string;
    gates:
        out: out;
endsimple


//
// Keeps track of statistical info.
//
simple Stat
    parameters:
        no_comps: numeric const;
    gates:
        in: in[];
endsimple


//
// Destroys the messages.
//
simple FDDI_Sink
    gates:
        in: in;
endsimple


//
// Monitors the network
//
simple FDDI_Monitor
    gates:
        in: in;
        // out: out;
endsimple


//
// Ringwise Load Control. It changes the value of LoadMultiplier
//
simple LoadControl
    parameters:
        LoadControlFile: string;
endsimple


