module Polyline
{
    @class(ArrangeFigures);
    @display("bgb=1000,1000");

    @figure[pli01](type=polyline; points=10,0,30,18,100,0,200,5);
    @figure[pli02](type=polyline; points=10,0,30,18,100,0,200,5; lineColor=#00ff00);
    @figure[pli04](type=polyline; points=10,0,30,18,100,0,200,5; lineStyle=solid);
    @figure[pli05](type=polyline; points=10,0,30,18,100,0,200,5; lineStyle=dashed);
    @figure[pli06](type=polyline; points=10,0,30,18,100,0,200,5; lineStyle=dotted);
    @figure[pli10](type=polyline; points=10,0,30,18,100,0,200,5; lineWidth=3; lineStyle=solid);
    @figure[pli08](type=polyline; points=10,0,30,18,100,0,200,5; lineWidth=3; lineStyle=dashed);
    @figure[pli09](type=polyline; points=10,0,30,18,100,0,200,5; lineWidth=3; lineStyle=dotted);
    @figure[pli11](type=polyline; points=10,0,30,18,100,0,200,5; endArrowhead=simple);
    @figure[pli12](type=polyline; points=10,0,30,18,100,0,200,5; endArrowhead=triangle);
    @figure[pli13](type=polyline; points=10,0,30,18,100,0,200,5; endArrowhead=barbed);
    @figure[pli14](type=polyline; points=10,0,30,18,100,0,200,5; startArrowhead=simple);
    @figure[pli15](type=polyline; points=10,0,30,18,100,0,200,5; startArrowhead=triangle);
    @figure[pli16](type=polyline; points=10,0,30,18,100,0,200,5; startArrowhead=barbed);
    @figure[pli17](type=polyline; points=10,0,30,18,100,0,200,5; startArrowhead=triangle; endArrowhead=triangle);
    @figure[pli18](type=polyline; points=10,0,30,18,100,0,200,5; startArrowhead=triangle; endArrowhead=barbed);  //note: Tk doesn't support having different arrows at the two ends
    //TODO joinstyle, capstyle

    @figure[pli21](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true);
    @figure[pli22](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true; lineColor=#00ff00);
    @figure[pli24](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true; lineStyle=solid);
    @figure[pli25](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true; lineStyle=dashed);
    @figure[pli26](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true; lineStyle=dotted);
    @figure[pli27](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true; lineWidth=3; lineStyle=solid);
    @figure[pli28](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true; lineWidth=3; lineStyle=dashed);
    @figure[pli29](type=polyline; points=10,0,30,18,100,0,200,5; smooth=true; lineWidth=3; lineStyle=dotted);
}

