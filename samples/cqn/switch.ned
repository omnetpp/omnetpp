//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Sends received packets to a randomly chosen output. Gates are
// chosen with uniform probability.
//
simple Switch
    gates:
        in: in;
        out: out[];
endsimple


