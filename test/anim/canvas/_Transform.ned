module Transform
{
    parameters:
        @display("bgb=800,500");

        @figure[g](type=group; transform=translate(100,100));
        @figure[g.origin](type=oval; pos=0,0; size=6,6);
        @figure[g.r0](type=rectangle; pos=100,100; size=100,100; fillColor=green);
        @figure[g.r1](type=rectangle; pos=100,100; size=100,100; transform=rotate(45); fillColor=blue);
        @figure[g.r2](type=rectangle; pos=100,100; size=100,100; transform=rotate(45,150,150); fillColor=red);
}

