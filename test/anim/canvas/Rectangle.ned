module Rectangle
{
    @class(ArrangeFigures);
    @display("bgb=1000,1200");

    @figure[r01](type=rectangle; pos=10,0; size=190,10);
    @figure[r02](type=rectangle; pos=10,0; size=190,10; fillColor=#0000ff);
    @figure[r03](type=rectangle; pos=10,0; size=190,10; lineColor=#ff0000);
    @figure[r04](type=rectangle; pos=10,0; size=190,10; lineWidth=3);
    @figure[r05](type=rectangle; pos=10,0; size=190,10; lineStyle=solid);
    @figure[r06](type=rectangle; pos=10,0; size=190,10; lineStyle=dotted);
    @figure[r07](type=rectangle; pos=10,0; size=190,10; lineStyle=dashed);
    @figure[r08](type=rectangle; pos=10,0; size=190,10; fillColor=#0000ff; lineColor=#ff0000; lineWidth=3; lineStyle=dashed);
    @figure[r09](type=rectangle; pos=10,0; size=190,10; fillColor=#00ff00; cornerRadius=3);
    @figure[r10](type=rectangle; pos=10,0; size=190,10; fillColor=#00ff00; cornerRadius=10,4);
    @figure[r11](type=rectangle; pos=10,0; size=190,10; fillColor=#00ff00; cornerRadius=12);
}

