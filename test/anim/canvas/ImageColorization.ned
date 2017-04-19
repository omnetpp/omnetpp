module ImageColorization
{
    @display("bgb=2500,1200");
    @class(ArrangeFigures);
    @arrange(startPos=70,100; lineSpacing=150; columnWidth=150; drawAnchor=false);

    @addColumn[0](tint=red,0);
    @addColumn[1](tint=red,0.2);
    @addColumn[2](tint=red,0.5);
    @addColumn[3](tint=red,0.7);
    @addColumn[4](tint=red,1);
    @addColumn[5](tint=green,1);
    @addColumn[6](tint=blue,1);

    @figure[im1](type=image; pos=0,0; image=device/device_l);
    @figure[im2](type=image; pos=0,0; image=device/device_l; size=100,80);
    @figure[ic1](type=icon; pos=0,0; image=block/cogwheel);
    @figure[ic2](type=icon; pos=0,0; image=block/cogwheel; size=60,30);
    @figure[px1](type=pixmap; pos=0,0; resolution=40,40; fillColor=yellow);
    @figure[px2](type=pixmap; pos=0,0; resolution=40,40; fillColor=yellow; size=100,80);
}
