//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates data packets
//
simple HCGenerator
    parameters:
        num_stations: numeric,
        address: numeric,
        ia_time: numeric;
    gates:
        out: out;
endsimple


//
// Destroys packets and creates statistics
//
simple HCSink
    parameters:
        address: numeric;
    gates:
        in: in;
endsimple


