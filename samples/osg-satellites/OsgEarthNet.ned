//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// The osgEarth Demo network.
//
network OsgEarthNet
{
    parameters:
        @display("bgb=500,500;bg=black");

        int numSats = default(0);
        int numStats = default(0);

    submodules:
        osgEarthScene: OsgEarthScene {
            @display("is=vl;p=300,300");
        }
        sat[numSats]: Satellite {
            @display("p=204,150");
        }
        channelController: ChannelController {
            @display("p=243,31");
        }
        stat[numStats]: GroundStation {
            @display("p=250,250");
        }
        clock: Clock {
            @display("p=44.352,41.328003");
        }
}
