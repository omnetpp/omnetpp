//--------------------------------------------------
// File: token.msg
//
// Message definitions for Token Ring
//
// Author: Andras Varga
//--------------------------------------------------


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
    fields:
       int destination;
       string data;
};


//
// The token.
//
message TRToken
{
    fields:
};


//
// Data frame on the Token Ring LAN.
//
message TRFrame
{
    fields:
       int source;
       int destination;
};

