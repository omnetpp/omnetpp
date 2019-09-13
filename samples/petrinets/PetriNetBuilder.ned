//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Reads a PNML (Petri Net Markup Language) file, and sets up the Petri net
// under the compound module that contains this module.
//
simple PetriNetBuilder
{
    parameters:
        @display("i=block/cogwheel_s");
        xml pnmlFile;  // PNML file to read the network from
        string id = default(""); // id of the network in the file; if empty, the first network will be chosen
        string placeNedType = default("Place");
        string transitionNedType = default("Transition");
        string arcNedType = default("Arc");
        string transitionSchedulerModule = default("scheduler");
}
