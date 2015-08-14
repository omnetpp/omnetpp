//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// The OpenSceneGraph indoor demo.
//
network OsgIndoorNet
{
    parameters:
        @display("bgb=$playgroundWidth,$playgroundHeight");
        double playgroundWidth @unit("m") = default(300m);
        double playgroundHeight @unit("m") = default(150m);
        int numPeople = default(0);  // the number of people in the office

    submodules:
        osgScene: OsgScene {
            @display("p=74,31");
        }
        man[numPeople]: Person {
            @display("p=144,224");
        }
}
