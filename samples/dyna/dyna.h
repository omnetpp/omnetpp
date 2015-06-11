//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

// message kind values (packet types):
enum {
    CONN_REQ,
    CONN_ACK,
    DATA_QUERY,
    DATA_RESULT,
    DISC_REQ,
    DISC_ACK
};

