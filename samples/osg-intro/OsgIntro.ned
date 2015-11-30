//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

simple OsgScene
{
    string scene;
    bool earthViewer = default(false);  // use normal (trackpoint) viewer by default
}

network OsgIntro
{
    submodules:
        osgScene: OsgScene {
            @display("p=147,96");
        }
}
