//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Loads the Earth model from the given file, and at runtime rotates 
// the Earth 360deg/24h.
//
simple OsgEarthScene
{
    @display("i=misc/globe");
    string scene;
}
