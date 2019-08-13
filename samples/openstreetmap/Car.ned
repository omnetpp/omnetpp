//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


module Car
{
    parameters:
        @display("i=misc/node_vs");
    submodules:
        driver: Driver {@display("p=100,70");}
        routePlanner: RoutePlanner {@display("p=200,70");};
}

simple Driver
{
    parameters:
        @class(osm::Driver);
        @display("i=block/user");
        string mapModule = default("map");
        string routePlannerModule = default("^.routePlanner");
}

simple RoutePlanner
{
    parameters:
        @class(osm::RoutePlanner);
        @display("i=block/network2");
        string mapModule = default("map");
}
