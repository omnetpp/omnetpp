module Tooltip
{
    @display("bgb=1000,1900");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=120; columnWidth=120; drawBBox=false; drawReferenceLine=false);

    @addColumn[1](tooltip="Single-line tooltip");
    @addColumn[2](tooltip="Multi-line\ntooltip");

    @figure[li](type=line; points=0,0,80,50);
    @figure[li](type=line; points=0,0,80,50);
    @figure[ar](type=arc; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl](type=polyline; points=10,0,30,20,50,0,80,60);
    @figure[re](type=rectangle; pos=0,0; size=80,60);
    @figure[rr](type=rectangle; pos=0,0; size=80,60; cornerRadius=10);
    @figure[ov](type=oval; pos=0,0; size=80,60);
    @figure[ps](type=pieslice; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pg](type=polygon; points=10,0,30,80,60,30,100,30,100,0);
    @figure[ri](type=ring; pos=0,0; size=110,60; innerSize=80,30);
    @figure[pa](type=path; path="M 0 50 L 10 0 L 80 80 Z");
    @figure[t01](type=text; pos=0,0; text="Text figure");
    @figure[t03](type=label; pos=0,0; text="Label figure");
    @figure[im2](type=image; pos=0,0; image=device/antennatower_l; size=100,80);
    @figure[ic2](type=icon; pos=0,0; image=block/cogwheel; size=60,30);
    @figure[px1](type=pixmap; pos=0,0; resolution=40,40; fillColor=yellow);
}

