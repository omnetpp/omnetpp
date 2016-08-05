module FillOpacity
{
    @display("bgb=1800,1200;bgi=background/terrain,s");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=120; columnWidth=120; drawBBox=false; drawReferenceLine=false);

    @addColumn[1](fillOpacity=0);
    @addColumn[2](fillOpacity=.1);
    @addColumn[3](fillOpacity=.2);
    @addColumn[4](fillOpacity=.5);
    @addColumn[5](fillOpacity=0.7);
    @addColumn[6](fillOpacity=1);
    @addColumn[6](lineWidth=15; lineOpacity=0.3; fillColor=green; fillOpacity=0.3);

    @figure[re](type=rectangle; pos=0,0; size=80,60; lineWidth=5; fillColor=blue);
    @figure[rr](type=rectangle; pos=0,0; size=80,60; cornerRadius=10; lineWidth=5; fillColor=blue);
    @figure[ov](type=oval; pos=0,0; size=80,60; lineWidth=5; fillColor=blue);
    @figure[ps](type=pieslice; pos=0,0; size=80,80; startAngle=0; endAngle=270; lineWidth=5; fillColor=blue);
    @figure[pg](type=polygon; points=10,0,30,80,60,30,100,30,100,0; lineWidth=5; fillColor=blue);
    @figure[ri](type=ring; pos=0,0; size=110,60; innerSize=80,30; lineWidth=5; fillColor=blue);
    @figure[pa](type=path; path="M 0 50 L 10 0 L 80 80 Z"; lineWidth=5; fillColor=blue);
}

