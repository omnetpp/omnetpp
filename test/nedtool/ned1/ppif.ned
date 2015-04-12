//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Point-to-point line interface.
//
// Frames that arrive on gate "in" are queued up until they get transmitted.
// The maximum queue size (in bits and in frames) can be specified as
// parameters; excess frames are simply discarded and recorded as statistics.
//
simple PointToPointIF
    parameters:
        frameCapacity: numeric const; // max number of frames; 0 means no limit
    gates:
        in: in, lineIn;
        out: out, lineOut;
endsimple


