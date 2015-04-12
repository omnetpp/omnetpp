//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A real-time interface
//
simple TelnetClient
    parameters:
        sendIaTime: numeric,
        addr: numeric const,
        srvAddr: numeric const;
    gates:
        in: in;
        out: out;
endsimple



