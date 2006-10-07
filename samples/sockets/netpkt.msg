//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Packet type that can be relayed by a Cloud.
//
message NetPkt
{
    fields:
        int srcAddress;
        int destAddress;
};
