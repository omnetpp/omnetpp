module LineWidth
{
    @display("bgb=1400,1300");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=120; columnWidth=120; drawBBox=false; drawReferenceLine=false);

    @addColumn[2](lineWidth=1);
    @addColumn[3](lineWidth=2);
    @addColumn[4](lineWidth=5);
    @addColumn[6](lineWidth=1; zoomLineWidth=true);
    @addColumn[7](lineWidth=2; zoomLineWidth=true);
    @addColumn[8](lineWidth=5; zoomLineWidth=true);

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

