//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Network for parsim testing
//
network Tictoc1
{
    submodules:
        tic: Tic {outputGate="g$o"; delete=true;}
        toc: Tic {outputGate="g$o"; delete=true;}
    connections:
        tic.g <--> { delay = 100ms; } <--> toc.g;
}

