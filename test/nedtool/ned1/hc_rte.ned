//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Implements deflection routing in a Hypercube network
//
simple HCRouter
    parameters:
        address: numeric,
        dim: numeric;
    gates:
        in: in[];
        in: from_gen;
        out: out[];
        out: to_sink;
endsimple


