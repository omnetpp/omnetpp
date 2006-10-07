//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

cplusplus {{
#include "netpkt_m.h"
}};

message NetPkt;

//
// Message class for representing a HTTP request or reply.
// For purposes of this model, we employ a very simple notion of HTTP:
// every HTTP request and reply is modelled with a single HTTPMsg packet.
// (We don't care about TCP connection setup and teardown, about TCP acks,
// and also ignore the fact that long requests and replies may span
// several packets in real life).
//
message HTTPMsg extends NetPkt
{
    fields:
        string payload;
};

