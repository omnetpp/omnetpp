//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

simple OsgScene
{
    string sceneFile; // model file to load via osgDB::readNodeFile()
    bool isEarthStyle = default(false);  // selects viewer style
}

network OsgIntro
{
    submodules:
        osgScene: OsgScene {
            @display("p=147,96");
        }
}
