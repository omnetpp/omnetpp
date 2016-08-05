module Image
{
    @class(ArrangeFigures);
    @arrange(startPos=80,30; lineSpacing=120; columnWidth=220);
    @display("bgb=800,2000");

    @figure[it1](type=image; pos=0,0; anchor=nw; image=block/browser_l);
    @figure[it2](type=image; pos=0,0; anchor=nw; image=block/browser_l; transform=scale(2,1));
    @figure[it3](type=image; pos=0,0; anchor=nw; image=block/browser_l; transform=rotate(45,10,250));
    @figure[it4](type=image; pos=0,0; anchor=nw; image=block/browser_l; transform=skewx(0.5,350));
    @figure[it5](type=image; pos=0,0; anchor=nw; image=block/browser_l; transform=skewy(0.5,10));

    @figure[is1](type=image; pos=0,0; anchor=nw; image=block/browser_l);
    @figure[is2](type=image; pos=0,0; anchor=nw; image=block/browser_l; size=100);
    @figure[is3](type=image; pos=0,0; anchor=nw; image=block/browser_l; size=,50);
    @figure[is4](type=image; pos=0,0; anchor=nw; image=block/browser_l; size=100,50);
}

