//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Required for the correct operation of the Petri Net model. Exactly one
// instance of this module type must be present in the network. 
//
simple TransitionScheduler
{
    parameters:
        @display("i=block/cogwheel_s");
}
