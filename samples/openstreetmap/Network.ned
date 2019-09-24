//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

network Network
{
    parameters:
        @display("bgb=1000,800");
        @figure[submodules](type=group;zIndex=100);
        int numCars = default(100);
    submodules:
        map: OpenStreetMap {@display("p=50,50");}
        car[numCars]: Car {@display("p=100,50");}
}
