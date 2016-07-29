module Pieslice
{
    @class(ArrangeFigures);
    @display("bgb=1000,1200");

    @figure[pc1](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250);
    @figure[pc2](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250; fillColor=#0000ff);
    @figure[pc3](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineColor=#ff0000);
    @figure[pc4](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineWidth=3);
    @figure[pc5](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineStyle=solid);
    @figure[pc6](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineStyle=dotted);
    @figure[pc7](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineStyle=dashed);
    @figure[pc8](type=pieslice; pos=10,0; size=40,10; startAngle=0; endAngle=250; fillColor=#0000ff; lineColor=#ff0000; lineWidth=3; lineStyle=dashed);
}

