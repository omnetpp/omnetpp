//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Dynamically sets up a network, with the topology coming from a MySQL database.
//
simple cMySQLNetBuilder
    parameters:
        connectPrefix: string,
        networkName: string,
        parentModule: string;
endsimple
