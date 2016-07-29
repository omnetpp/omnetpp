module Image
{
    parameters:
        @display("bgb=800,500");

        @figure[i1](type=image; pos=10,50; anchor=nw; image=block/browser_l);
        @figure[i2](type=image; pos=10,150; anchor=nw; image=block/browser_l; transform=scale(3,1));
        @figure[i3](type=image; pos=10,250; anchor=nw; image=block/browser_l; transform=rotate(45,10,250));
        @figure[i4](type=image; pos=10,350; anchor=nw; image=block/browser_l; transform=skewx(0.5,350));
        @figure[i5](type=image; pos=10,450; anchor=nw; image=block/browser_l; transform=skewy(0.5,10));

        @figure[is1](type=image; pos=600,50; anchor=nw; image=block/browser_l);
        @figure[is2](type=image; pos=600,150; anchor=nw; image=block/browser_l; size=30);
        @figure[is3](type=image; pos=600,250; anchor=nw; image=block/browser_l; size=,30);
        @figure[is4](type=image; pos=600,350; anchor=nw; image=block/browser_l; size=50,110);
}

