module Ring
{
    @class(ArrangeFigures);
    @display("bgb=1000,1200");

    @figure[ring1](type=ring; pos=10,0; size=100,10; innerSize=6);
    @figure[ring2](type=ring; pos=10,0; size=100,10; innerSize=40,4; fillColor=#0000ff);
    @figure[ring3](type=ring; pos=10,0; size=100,10; innerSize=4,8; lineColor=#ff0000; fillColor=#302010);
    @figure[ring4](type=ring; pos=10,0; size=100,10; innerSize=40,8; lineWidth=3);
    @figure[ring5](type=ring; pos=10,0; size=100,10; lineStyle=solid; lineWidth=2);
    @figure[ring6](type=ring; pos=10,0; size=100,10; innerSize=20,8; lineStyle=dotted);
    @figure[ring7](type=ring; pos=10,0; size=100,10; innerSize=30,4; lineStyle=dashed);
    @figure[ring8](type=ring; pos=10,0; size=100,10; innerSize=20,4; fillColor=#0000ff; lineColor=#ff0000; lineWidth=3; lineStyle=dashed);
}

