//
// Test for the clipChildren flag of shape figures. Check visually, also with
// zooming, and check that tooltips do not appear over clipped-away areas.
//
module Clipping
{
    parameters:
        @class(ClippingAnimator);
        @display("bgb=1300,1100");
        int numBalls = default(500); // for case 17

        // basic: an oval sticking out of a rectangle; the rectangle's own (thick) outline must not be clipped
        @figure[t1](type=text; pos=20,20; text="1. oval clipped to rect, outline not clipped");
        @figure[c1](type=rectangle; bounds=20,40,200,120; lineWidth=8; lineColor=darkblue; fillColor=lightyellow; clipChildren=true);
        @figure[c1.oval](type=oval; bounds=120,80,200,160; fillColor=orange; tooltip="tooltip must not appear outside the rect");

        // rounded corners
        @figure[t2](type=text; pos=330,20; text="2. rounded corners clip");
        @figure[c2](type=rectangle; bounds=330,40,200,120; cornerRadius=40; fillColor=lightyellow; clipChildren=true);
        @figure[c2.fill](type=rectangle; bounds=300,20,260,160; fillColor=orange; lineColor=);

        // non-shape children: text, label, image, line
        @figure[t3](type=text; pos=640,20; text="3. oval clips text, label, icon, line");
        @figure[c3](type=oval; bounds=640,40,220,120; fillColor=lightyellow; clipChildren=true);
        @figure[c3.text](type=text; pos=600,80; text="This text is longer than the oval"; font=,14);
        @figure[c3.label](type=label; pos=600,110; text="This label is longer than the oval");
        @figure[c3.icon](type=icon; pos=850,140; image=block/app);
        @figure[c3.line](type=line; points=620,40,880,160; lineWidth=4; lineColor=red);

        // ring: the hole is excluded from the clip region
        @figure[t4](type=text; pos=960,20; text="4. ring: hole is not part of the region");
        @figure[c4](type=ring; bounds=960,40,160,160; innerSize=80,80; lineColor=darkblue; clipChildren=true);
        @figure[c4.fill](type=rectangle; bounds=940,60,220,60; fillColor=orange; lineColor=);

        // polygon with even-odd fill rule: the middle of the star is not part of the region
        @figure[t5](type=text; pos=20,240; text="5. polygon (evenodd star)");
        @figure[c5](type=polygon; points=120,260,60,440,210,320,30,320,180,440; fillRule=evenodd; lineColor=darkblue; clipChildren=true);
        @figure[c5.fill](type=rectangle; bounds=0,330,240,60; fillColor=orange; lineColor=);

        // path
        @figure[t6](type=text; pos=330,240; text="6. path");
        @figure[c6](type=path; path="M 330 260 L 530 260 Q 430 360 530 440 L 330 440 Z"; lineColor=darkblue; clipChildren=true);
        @figure[c6.fill](type=rectangle; bounds=310,330,260,60; fillColor=orange; lineColor=);

        // nested clipping: intersection of the rect and the oval
        @figure[t7](type=text; pos=640,240; text="7. nested: rect AND oval");
        @figure[c7](type=rectangle; bounds=640,260,200,160; lineColor=darkblue; clipChildren=true);
        @figure[c7.inner](type=oval; bounds=740,300,200,160; lineColor=darkgreen; clipChildren=true);
        @figure[c7.inner.fill](type=rectangle; bounds=620,240,340,240; fillColor=orange; lineColor=);

        // transformed clipper: the clip region rotates with the figure, but not with the child's own transform
        @figure[t8](type=text; pos=960,240; text="8. rotated clipper, skewed child");
        @figure[c8](type=rectangle; bounds=1000,280,140,100; fillColor=lightyellow; transform=rotate(20,1070,330); clipChildren=true);
        @figure[c8.fill](type=rectangle; bounds=960,310,220,40; fillColor=orange; lineColor=; transform=skewx(0.5,330));

        // stacking order: clipping does not affect it. The child with zIndex=10 must be ABOVE the green bar (zIndex=5)
        // outside the clipper; the child with zIndex=-10 must be BELOW the clipper's half-transparent yellow fill.
        // Both must be clipped.
        @figure[t9](type=text; pos=20,480; text="9. orange above green bar; blue under yellow");
        @figure[c9](type=rectangle; bounds=20,500,240,140; fillColor=lightyellow; fillOpacity=0.7; clipChildren=true);
        @figure[c9.high](type=rectangle; bounds=0,520,300,60; fillColor=orange; zIndex=10);
        @figure[c9.low](type=rectangle; bounds=60,600,40,60; fillColor=blue; zIndex=-10);
        @figure[bar9](type=rectangle; bounds=140,490,40,160; fillColor=green; zIndex=5);

        // invisible clipper (neither outlined nor filled)
        @figure[t10](type=text; pos=330,480; text="10. invisible clipper");
        @figure[c10](type=rectangle; bounds=330,500,200,140; lineColor=; clipChildren=true);
        @figure[c10.oval](type=oval; bounds=280,520,300,100; fillColor=orange);

        // window fixed in screen pixels: panel + clipping rectangle; must not scale with zoom
        @figure[t11](type=text; pos=640,480; text="11. panel + clip rect: not zoomed");
        @figure[p11](type=panel; pos=640,500);
        @figure[p11.c](type=rectangle; bounds=0,0,200,100; fillColor=lightyellow; clipChildren=true);
        @figure[p11.c.text](type=text; pos=10,40; text="Zoom-immune clipped window content"; font=,16);

        // clipChildren=false must render exactly as before
        @figure[t12](type=text; pos=960,480; text="12. clipChildren=false: not clipped");
        @figure[c12](type=rectangle; bounds=960,500,200,140; fillColor=lightyellow; clipChildren=false);
        @figure[c12.oval](type=oval; bounds=1060,540,200,140; fillColor=orange);

        // animated by ClippingAnimator
        @figure[t13](type=text; pos=20,700; text="13. animated: rotating clipper, moving child");
        @figure[c13](type=rectangle; bounds=60,740,200,120; fillColor=lightyellow; clipChildren=true);
        @figure[c13.oval](type=oval; bounds=60,760,80,80; fillColor=orange);
        @figure[t14](type=text; pos=330,700; text="14. animated: clipper geometry changes");
        @figure[c14](type=rectangle; bounds=330,740,200,120; fillColor=lightyellow; clipChildren=true);
        @figure[c14.fill](type=oval; bounds=310,720,240,160; fillColor=orange);
        @figure[t15](type=text; pos=640,700; text="15. animated: clipChildren toggles every 2s");
        @figure[c15](type=rectangle; bounds=640,740,200,120; fillColor=lightyellow; clipChildren=true);
        @figure[c15.oval](type=oval; bounds=740,780,200,100; fillColor=orange);
        @figure[t16](type=text; pos=960,700; text="16. animated: clipper zIndex toggles vs green bar");
        @figure[c16](type=rectangle; bounds=960,740,200,120; fillColor=lightyellow; clipChildren=true);
        @figure[c16.fill](type=rectangle; bounds=980,760,160,180; zIndex=2; fillColor=orange);
        @figure[bar16](type=rectangle; bounds=1040,720,40,160; fillColor=green; zIndex=1);

        // rendering scalability: lots of balls (see numBalls) moving all over the module area, as children
        // of a small window whose clipping is toggled; the balls are created by ClippingAnimator
        @figure[t17](type=text; pos=20,920; text="17. scalability: many balls under a small window, clipChildren toggles every 5s");
        @figure[c17](type=rectangle; bounds=60,950,160,100; lineWidth=2; cornerRadius=40; fillColor=lightyellow; clipChildren=true);

    submodules:
        ticker: ClippingTicker {
            @display("p=1250,40");
        }
}

// Keeps the simulation running so that the animated cases move
simple ClippingTicker
{
}
