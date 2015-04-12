//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Queue server. It serves multiple input queues (QPassiveQueue),
// as priority queue.
//
simple QServer
    parameters:
        serviceTime: numeric;
    gates:
        in: in[];
        out: out;
endsimple


