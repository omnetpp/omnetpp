module ImageOpacity
{
    @display("bgb=2500,1200");
    @class(ArrangeFigures);
    @arrange(startPos=70,100; lineSpacing=150; columnWidth=150; drawAnchor=false);

    @addColumn[0](opacity=0);
    @addColumn[1](opacity=0.2);
    @addColumn[2](opacity=0.5);
    @addColumn[3](opacity=0.7);
    @addColumn[4](opacity=1);

    @figure[im1](type=image; pos=0,0; image=device/antennatower_l);
    @figure[im2](type=image; pos=0,0; image=device/antennatower_l; size=100,80);
    @figure[ic1](type=icon; pos=0,0; image=block/cogwheel);
    @figure[ic2](type=icon; pos=0,0; image=block/cogwheel; size=60,30);
    @figure[px1](type=pixmap; pos=0,0; resolution=40,40; fillColor=yellow);
    @figure[px2](type=pixmap; pos=0,0; resolution=40,40; fillColor=yellow; size=100,80);
}
