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
// Message class for representing a Telnet packet (bytes carried in TCP).
// For purposes of this model, we don't care about TCP connection setup
// and teardown, about TCP acks, or finite IP packet size.
//
message TelnetPkt extends NetPkt
{
    fields:
        string payload;
};

