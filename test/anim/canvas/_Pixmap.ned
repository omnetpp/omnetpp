simple PixmapAnimator
{
}

module Pixmap
{
    parameters:
        @display("bgb=800,500;bgi=background/green,s");
        @figure[note](type=text; pos=100,40; text="Click Run to see the pixmap change");
        @figure[pixmap](type=pixmap; pos=50,50; size=600,400; anchor=nw; resolution=50,20);
        @figure[bg](type=image; pos=300,200; zIndex=-1; size=200,120; anchor=nw; image=abstract/router);
    submodules:
        animator: PixmapAnimator {
            @display("p=700,20;i=block/cogwheel_vs");
        }
}

