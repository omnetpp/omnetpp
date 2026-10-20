module ZOrder
{
    @display("bgb=1200,1850");

    @figure[t0] (type=text; text=Figures should appear cascaded; pos=100,100);

    @figure[o3] (type=rectangle; fillColor=#40ff40; pos=300,300; size=500,500; anchor=nw; zIndex=3);
    @figure[o3a](type=rectangle; fillColor=#40ff40; pos=330,330; size=500,500; anchor=nw; zIndex=3);
    @figure[o3b](type=rectangle; fillColor=#40ff40; pos=360,360; size=500,500; anchor=nw; zIndex=3);
    @figure[o4] (type=rectangle; fillColor=#60ff60; pos=400,400; size=500,500; anchor=nw; zIndex=4);
    @figure[o1] (type=rectangle; fillColor=#00ff00; pos=500,500; size=500,500; anchor=nw; zIndex=1);
    @figure[o5] (type=rectangle; fillColor=#80ff80; pos=500,500; size=500,500; anchor=nw; zIndex=5);
    @figure[o2] (type=rectangle; fillColor=#20ff20; pos=200,200; size=500,500; anchor=nw; zIndex=2);

    // The stacking order is global: the children of two sibling groups interleave according to their
    // effective Z-index (the sum of their own and their ancestors' zIndex). Group "gb" has zIndex=1, so
    // its children are at 2, 4, 6, and the ones of "ga" at 1, 3, 5.
    @figure[t1] (type=text; text="Red and blue figures should alternate in a cascade, each one above the previous one"; pos=100,1050);

    @figure[ga] (type=group);
    @figure[ga.r1](type=rectangle; fillColor=#ff8080; pos=100,1100; size=300,300; anchor=nw; zIndex=1);
    @figure[ga.r3](type=rectangle; fillColor=#ff4040; pos=240,1240; size=300,300; anchor=nw; zIndex=3);
    @figure[ga.r5](type=rectangle; fillColor=#ff0000; pos=380,1380; size=300,300; anchor=nw; zIndex=5);

    @figure[gb] (type=group; zIndex=1);
    @figure[gb.b2](type=rectangle; fillColor=#8080ff; pos=170,1170; size=300,300; anchor=nw; zIndex=1);
    @figure[gb.b4](type=rectangle; fillColor=#4040ff; pos=310,1310; size=300,300; anchor=nw; zIndex=3);
    @figure[gb.b6](type=rectangle; fillColor=#0000ff; pos=450,1450; size=300,300; anchor=nw; zIndex=5);
}

