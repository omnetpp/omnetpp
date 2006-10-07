//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


enum DynaPacketType
{
    DYNA_CONN_REQ = 0;
    DYNA_CONN_ACK = 1;
    DYNA_DISC_REQ = 2;
    DYNA_DISC_ACK = 3;
    DYNA_DATA = 4;
};

message DynaPacket
{
    fields:
        int srcAddress;
        int destAddress;
        int serverProcId;
};

message DynaDataPacket extends DynaPacket
{
    fields:
        string payload;
};

