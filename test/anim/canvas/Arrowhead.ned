module Arrowhead
{
    @display("bgb=2100,2600");
    @class(ArrangeFigures);
    @arrange(startPos=30,100; lineSpacing=140; columnWidth=140; drawBBox=false; drawReferenceLine=false);

    @addColumn[1](startArrowhead=none);
    @addColumn[2](startArrowhead=simple);
    @addColumn[3](startArrowhead=triangle);
    @addColumn[4](startArrowhead=barbed);
    @addColumn[5](endArrowhead=none);
    @addColumn[6](endArrowhead=simple);
    @addColumn[7](endArrowhead=triangle);
    @addColumn[8](endArrowhead=barbed);
    @addColumn[a6](startArrowhead=triangle; endArrowhead=simple);
    @addColumn[a8](startArrowhead=barbed; endArrowhead=triangle);

    @figure[li1](type=line; points=0,0,80,50);
    @figure[ar1](type=arc; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl1](type=polyline; points=10,0,30,80,80,60);

    @figure[li2](type=line; lineWidth=2; points=0,0,80,50);
    @figure[ar2](type=arc; lineWidth=2; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl2](type=polyline; lineWidth=2; points=10,0,30,80,80,60);

    @figure[li3](type=line; lineWidth=10; points=0,0,80,50);
    @figure[ar3](type=arc; lineWidth=10; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl3](type=polyline; lineWidth=10; points=10,0,30,80,80,60);
    
    @figure[li1z](type=line; zoomLineWidth=true; points=0,0,80,50);
    @figure[ar1z](type=arc; zoomLineWidth=true; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl1z](type=polyline; points=10,0,30,80,80,60);

    @figure[li2z](type=line; zoomLineWidth=true; lineWidth=2; points=0,0,80,50);
    @figure[ar2z](type=arc; lineWidth=2; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl2z](type=polyline; lineWidth=2; points=10,0,30,80,80,60);

    @figure[li3z](type=line; zoomLineWidth=true; lineWidth=10; points=0,0,80,50);
    @figure[ar3z](type=arc; zoomLineWidth=true; lineWidth=10; pos=0,0; size=80,80; startAngle=0; endAngle=270);
    @figure[pl3z](type=polyline; zoomLineWidth=true; lineWidth=10; points=10,0,30,80,80,60);
}

