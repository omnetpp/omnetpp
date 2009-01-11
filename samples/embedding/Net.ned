//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Source+Sink example
//
network Net
{
    submodules:
        gen: Source;
        sink: Sink;
    connections:
        gen.out --> sink.in;
}


