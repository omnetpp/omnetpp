module LineJoin
{
    @display("bgb=1800,1200");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=120; columnWidth=120; drawBBox=false; drawReferenceLine=false);

    @addColumn[1](lineWidth=15; joinStyle=miter);
    @addColumn[2](lineWidth=15; joinStyle=bevel);
    @addColumn[3](lineWidth=15; joinStyle=round);

    @figure[pl](type=polyline; points=10,0,30,20,50,0,80,60);
    @figure[pg](type=polygon; points=10,0,30,80,60,30,100,30,100,0);
    @figure[pa](type=path; path="M 0 50 L 10 0 L 80 80 Z");
}

