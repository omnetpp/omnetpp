module FilledAndOutlined
{
    @display("bgb=1800,1200;bgi=background/terrain,s");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=120; columnWidth=120; drawBBox=false; drawReferenceLine=false);

    // the isFilled/isOutlined flags are controlled by the corresponding color properties
    @addColumn[3](lineColor=; fillColor=);
    @addColumn[1](lineColor=black; fillColor=);
    @addColumn[4](lineColor=; fillColor=blue);
    @addColumn[2](lineColor=black; fillColor=blue);

    @figure[re](type=rectangle; pos=0,0; size=80,60; lineWidth=5);
    @figure[rr](type=rectangle; pos=0,0; size=80,60; cornerRadius=10; lineWidth=5);
    @figure[ov](type=oval; pos=0,0; size=80,60; lineWidth=5);
    @figure[ps](type=pieslice; pos=0,0; size=80,80; startAngle=0; endAngle=270; lineWidth=5);
    @figure[pg](type=polygon; points=10,0,30,80,60,30,100,30,100,0; lineWidth=5);
    @figure[ri](type=ring; pos=0,0; size=110,60; innerSize=80,30; lineWidth=5);
    @figure[pa](type=path; path="M 0 50 L 10 0 L 80 80 Z"; lineWidth=5);
}

