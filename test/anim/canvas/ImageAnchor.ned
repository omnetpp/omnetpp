module ImageAnchor
{
    @display("bgb=2500,1000");
    @class(ArrangeFigures);
    @arrange(startPos=70,100; lineSpacing=150; columnWidth=150);

    @addColumn[0](anchor=c);
    @addColumn[1](anchor=n);
    @addColumn[2](anchor=nw);
    @addColumn[3](anchor=w);
    @addColumn[4](anchor=sw);
    @addColumn[5](anchor=s);
    @addColumn[6](anchor=se);
    @addColumn[7](anchor=e);
    @addColumn[8](anchor=ne);

    @figure[im1](type=image; pos=0,0; image=device/antennatower_l);
    @figure[im2](type=image; pos=0,0; image=device/antennatower_l; size=100,80);
    @figure[ic1](type=icon; pos=0,0; image=block/cogwheel);
    @figure[ic2](type=icon; pos=0,0; image=block/cogwheel; size=60,30);
    @figure[px1](type=pixmap; pos=0,0; resolution=40,40; fillColor=yellow);
    @figure[px2](type=pixmap; pos=0,0; resolution=40,40; fillColor=yellow; size=100,80);
}
