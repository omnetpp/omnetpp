//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


network CarDemo
{
    parameters:
       @display("bgb=800,500");
       @figure[road] (type=polygon; points=100,200, 200,100, 300,50, 500,50, 650,100, 700,200, 600,300, 500,350, 400,350, 200,350, 100,300; smooth=true; lineWidth=60; zoomLineWidth=true; lineColor=grey);
       @figure[paint](type=polygon; points=100,200, 200,100, 300,50, 500,50, 650,100, 700,200, 600,300, 500,350, 400,350, 200,350, 100,300; smooth=true; lineWidth=3; zoomLineWidth=true; lineColor=white; lineStyle=dashed);
       @figure[trail](type=polyline; lineWidth=5; lineOpacity=0.6; lineColor=orange);
       @figure[car](type=image; image="car-top-view"; pos=0,0; anchor=c);
       @figure[car.antenna](type=image; image="antenna"; pos=-10,-40; anchor=nw);
       @figure[car.antenna.beam](type=path; path="M 10 0 L 10 -20 L 1200 -100 L 1200 100 L 10 20 Z"; fillColor=blue; fillOpacity=0.1; lineColor=blue; lineOpacity=0.1);
       @figure[status](type=group; transform=translate(700,20));
       // or, for non-zooming status area: @figure[status](type=panel; pos=700,20);
       @figure[status.bg](type=rectangle; pos=0,0; size=150,100; anchor=n; cornerRadius=10; fillColor=#fafdb9; fillOpacity=0.2; lineColor=#fafdb9; lineOpacity=0.4);
       @figure[status.distanceTitle](type=text; pos=0,20; text="Distance Travelled"; anchor=center);
       @figure[status.distance](type=text; pos=0,40; text="0m"; anchor=center; font=Arial,22);
       @figure[status.headingTitle](type=text; pos=0,70; text="Heading"; anchor=center);
       @figure[status.heading](type=text; pos=0,90; text="0"; anchor=center; font=Arial,22);

    submodules:
       animator: CarAnimator;
}
