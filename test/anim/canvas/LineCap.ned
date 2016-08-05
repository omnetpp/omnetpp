module LineCap
{
    @display("bgb=1800,1200");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=140; columnWidth=140; drawBBox=false; drawReferenceLine=false);

    @addColumn[1](lineWidth=20; capStyle=butt);
    @addColumn[2](lineWidth=20; capStyle=square);
    @addColumn[3](lineWidth=20; capStyle=round);

    @figure[li](type=line; points=0,0,80,50);
    @figure[ar](type=arc; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl](type=polyline; points=10,0,30,20,50,0,80,60);
    @figure[pa](type=path; path="M 0 50 L 10 0 L 80 80");
}

