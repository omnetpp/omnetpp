module LineOpacity
{
    @display("bgb=1300,1300;bgi=background/terrain,s");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=120; columnWidth=120; drawBBox=false; drawReferenceLine=false);

    @addColumn[1](lineWidth=10; lineOpacity=0);
    @addColumn[2](lineWidth=10; lineOpacity=0.2);
    @addColumn[3](lineWidth=10; lineOpacity=0.5);
    @addColumn[4](lineWidth=10; lineOpacity=0.7);
    @addColumn[5](lineWidth=10; lineOpacity=1);

    @figure[li](type=line; points=0,0,80,50);
    @figure[li](type=line; points=0,0,80,50);
    @figure[ar](type=arc; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl](type=polyline; points=10,0,30,20,50,0,80,60);
    @figure[re](type=rectangle; pos=0,0; size=80,60);
    @figure[rr](type=rectangle; pos=0,0; size=80,60; cornerRadius=10);
    @figure[ov](type=oval; pos=0,0; size=80,60);
    @figure[ps](type=pieslice; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pg](type=polygon; points=10,0,30,80,60,30,100,30,100,0);
    @figure[ri](type=ring; pos=0,0; size=110,60; innerSize=80,30);
    @figure[pa](type=path; path="M 0 50 L 10 0 L 80 80 Z");
}

