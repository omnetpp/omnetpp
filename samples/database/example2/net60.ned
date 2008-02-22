//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Represents a network node. This is just a dummy, but one could of course
// replace it with something useful as well.
//
module Node
{
    parameters:
        @display("i=misc/node_vs");
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

//
// The network will by dynamically constructed by the "builder" submodule,
// based on the topology in the MySQL database.
//
network Net60
{
    submodules:
        builder: cMySQLNetBuilder;
}

