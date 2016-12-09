module Icon
{
    @display("bgb=800,500");
    @class(ArrangeFigures);
    @arrange(startPos=50,50; lineSpacing=100; columnWidth=200);

    @figure[i1](type=icon; pos=0,0; anchor=nw; image=misc/sun);
    @figure[i2](type=icon; pos=0,0; anchor=nw; image=misc/sun_l; size=100);
    @figure[i3](type=icon; pos=0,0; anchor=nw; image=misc/sun_l; size=,50);
    @figure[i4](type=icon; pos=0,0; anchor=nw; image=misc/sun_l; size=100,50);
}

