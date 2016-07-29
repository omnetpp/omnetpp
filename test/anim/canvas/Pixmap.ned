simple PixmapAnimator
{
}

module Pixmap
{
    parameters:
        @display("bgb=800,500");
        @figure[note](type=text; pos=100,40; text="Click Run to see the pixmap change");
        @figure[bg1](type=image; pos=400,150; size=200,120; anchor=nw; image=abstract/router);
        @figure[bg2](type=path; path="M 100 200 Q 280 130 300 200 l 0 100 A 150 50 5 1 1 100 300 Z"; fillColor=lightblue; lineWidth=3; lineStyle=dotted);
        @figure[pixmap](type=pixmap; pos=50,50; size=600,400; anchor=nw; resolution=50,20);
    submodules:
        animator: PixmapAnimator {
            @display("p=700,20;i=block/cogwheel_vs");
        }
}

