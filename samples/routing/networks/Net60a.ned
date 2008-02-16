//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

//
// This network adds one extra connection to the net60 model (net60.ned).
//
network Net60a extends Net60
{
    connections:
        rte[55].port++ <--> rte[0].port++;
}

