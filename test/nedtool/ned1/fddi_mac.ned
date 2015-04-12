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
// MAC layer of an FDDI station
//
simple FDDI_MAC
    parameters:
        StationID: numeric,
        address: string;
    gates:
        in: in;
        in: from_LLC;
        out: out;
        out: to_LLC;
endsimple


//
// MAC layer of an FDDI station in a simple ring of identical nodes.
//
simple FDDI_MAC4Ring
    parameters:
        no_comps: numeric const,
        no_msg: numeric;
    gates:
        in: in;
        in: from_LLC;
        out: out;
        out: to_LLC;
endsimple


//
// MAC layer of an FDDI Sniffer.
//
simple FDDI_MAC4Sniffer
    parameters:
        StationID: numeric,
        address: string;
    gates:
        in: in;
        in: from_LLC;
        out: out;
        out: to_LLC;
endsimple

