//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// The actual hypercube network, instantiated as from the Hypercube
// topology template, substituting HypercubeNode for HCNode.
//
network HypercubeNetwork extends Hypercube
{
    parameters:
        dim = default(3);
        nodetype = "HypercubeNode";
}


