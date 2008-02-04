//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


enum TRTypes
{
    TR_TOKEN = 1;
    TR_FRAME = 2;
};


//
// Application data.
//
message TRApplicationData
{
    int destination;
    string data;
};


//
// The token.
//
message TRToken
{
};


//
// Data frame on the Token Ring LAN.
//
message TRFrame
{
    int source;
    int destination;
};

