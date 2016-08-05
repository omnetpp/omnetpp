module Pieslice
{
    @class(ArrangeFigures);
    @arrange(startPos=50,100; lineSpacing=120; columnWidth=70);
    @display("bgb=1200,1100");

    @addColumn[0](endAngle=-60; lineWidth=2; fillColor=green);
    @addColumn[1](endAngle=0; lineWidth=2; fillColor=green);
    @addColumn[2](endAngle=60; lineWidth=2; fillColor=green);
    @addColumn[3](endAngle=120; lineWidth=2; fillColor=green);
    @addColumn[4](endAngle=180; lineWidth=2; fillColor=green);
    @addColumn[5](endAngle=240; lineWidth=2; fillColor=green);
    @addColumn[6](endAngle=360; lineWidth=2; fillColor=green);
    @addColumn[7](endAngle=420; lineWidth=2; fillColor=green);

    @figure[ps01](type=pieslice; pos=0,0; size=50,100; startAngle=-60);
    @figure[ps02](type=pieslice; pos=0,0; size=50,100; startAngle=0);
    @figure[ps03](type=pieslice; pos=0,0; size=50,100; startAngle=60);
    @figure[ps04](type=pieslice; pos=0,0; size=50,100; startAngle=120);
    @figure[ps05](type=pieslice; pos=0,0; size=50,100; startAngle=180);
    @figure[ps06](type=pieslice; pos=0,0; size=50,100; startAngle=240);
    @figure[ps07](type=pieslice; pos=0,0; size=50,100; startAngle=360);
    @figure[ps08](type=pieslice; pos=0,0; size=50,100; startAngle=420);
}

