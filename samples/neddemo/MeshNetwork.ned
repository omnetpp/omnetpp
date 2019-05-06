//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2019 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple MeshNode extends Node
{
    parameters:
        double posX = default(uniform(0,100));
        double posY = default(uniform(0,100));

        @display("i=misc/node_vs;p=$posX,$posY");
    gates:
        inout g[];
}


//
// A network of nodes randomly placed where nodes closer than 'range' are connected.
// (i.e. the connectivity graph of a wireless mesh network)
//
network MeshNetwork
{
    parameters:
        int num @prompt("Number of nodes") = default(100);
        double range;  // nodes closer than this should be connected

    submodules:
        node[num] : MeshNode;

    connections allowunconnected:
        for i=0..(num-2), for j=(i+1)..(num-1) {
            node[i].g++ <--> node[j].g++ if pow(node[i].posX-node[j].posX,2)+pow(node[i].posY-node[j].posY,2) < range*range;
        }
}
