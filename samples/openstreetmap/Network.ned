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
        // Map and car figures are placed under this invisible rectangle, which clips them
        // to the map area. (OSM exports contain the complete ways that cross the bounding
        // box, so many streets and buildings extend far beyond it.)
        @figure[mapArea](type=rectangle; bounds=0,0,1000,800; lineColor=; clipChildren=true);
        @figure[submodules](type=group;zIndex=100);
        int numCars = default(100);
    submodules:
        map: OpenStreetMap {@display("p=50,50");}
        car[numCars]: Car {@display("p=100,50");}
}
