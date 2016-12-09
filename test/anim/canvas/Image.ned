module Image
{
    @display("bgb=800,500");
    @class(ArrangeFigures);
    @arrange(startPos=50,50; lineSpacing=100; columnWidth=200);

    @figure[i1](type=image; pos=0,0; anchor=nw; image=block/browser);
    @figure[i2](type=image; pos=0,0; anchor=nw; image=block/browser_l; size=100);
    @figure[i3](type=image; pos=0,0; anchor=nw; image=block/browser_l; size=,50);
    @figure[i4](type=image; pos=0,0; anchor=nw; image=block/browser_l; size=100,50);
}

