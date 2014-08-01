module Lines
{
    @class(ArrangeFigures);
    @display("bgb=1000,1000");

    @figure[li01](type=line;coords=10,5,200,5);
    @figure[li02](type=line;coords=10,5,200,5;lineColor=#00ff00);
    @figure[li04](type=line;coords=10,5,200,5;lineStyle=solid);
    @figure[li05](type=line;coords=10,5,200,5;lineStyle=dashed);
    @figure[li06](type=line;coords=10,5,200,5;lineStyle=dotted);
    @figure[li08](type=line;coords=10,5,200,5;lineWidth=3;lineStyle=dashed);
    @figure[li09](type=line;coords=10,5,200,5;lineWidth=3;lineStyle=dotted);
    @figure[li10](type=line;coords=10,5,200,5;lineWidth=3;lineStyle=dotted);
    @figure[li11](type=line;coords=10,5,200,5;endArrowhead=simple);
    @figure[li12](type=line;coords=10,5,200,5;endArrowhead=triangle);
    @figure[li13](type=line;coords=10,5,200,5;endArrowhead=barbed);
    @figure[li14](type=line;coords=10,5,200,5;startArrowhead=simple);
    @figure[li15](type=line;coords=10,5,200,5;startArrowhead=triangle);
    @figure[li16](type=line;coords=10,5,200,5;startArrowhead=barbed);
    @figure[li17](type=line;coords=10,5,200,5;startArrowhead=triangle;endArrowhead=triangle);
    @figure[li18](type=line;coords=10,5,200,5;startArrowhead=triangle;endArrowhead=barbed); //note: Tk doesn't support having different arrows at the two ends

    @figure[ar1](type=arc;coords=10,0,50,10;startAngle=0;endAngle=250);
    @figure[ar3](type=arc;coords=10,0,50,10;startAngle=0;endAngle=250;lineColor=#ff0000);
    @figure[ar4](type=arc;coords=10,0,50,10;startAngle=0;endAngle=250;lineWidth=3);
    @figure[ar5](type=arc;coords=10,0,50,10;startAngle=0;endAngle=250;lineStyle=solid);
    @figure[ar6](type=arc;coords=10,0,50,10;startAngle=0;endAngle=250;lineStyle=dotted);
    @figure[ar7](type=arc;coords=10,0,50,10;startAngle=0;endAngle=250;lineStyle=dashed);
    @figure[ar8](type=arc;coords=10,0,50,10;startAngle=0;endAngle=250;lineColor=#ff0000;lineWidth=3;lineStyle=dashed);

    @figure[pli01](type=polyline;coords=10,0,30,18,100,0,200,5);
    @figure[pli02](type=polyline;coords=10,0,30,18,100,0,200,5;lineColor=#00ff00);
    @figure[pli04](type=polyline;coords=10,0,30,18,100,0,200,5;lineStyle=solid);
    @figure[pli05](type=polyline;coords=10,0,30,18,100,0,200,5;lineStyle=dashed);
    @figure[pli06](type=polyline;coords=10,0,30,18,100,0,200,5;lineStyle=dotted);
    @figure[pli10](type=polyline;coords=10,0,30,18,100,0,200,5;lineWidth=3;lineStyle=solid);
    @figure[pli08](type=polyline;coords=10,0,30,18,100,0,200,5;lineWidth=3;lineStyle=dashed);
    @figure[pli09](type=polyline;coords=10,0,30,18,100,0,200,5;lineWidth=3;lineStyle=dotted);
    @figure[pli11](type=polyline;coords=10,0,30,18,100,0,200,5;endArrowhead=simple);
    @figure[pli12](type=polyline;coords=10,0,30,18,100,0,200,5;endArrowhead=triangle);
    @figure[pli13](type=polyline;coords=10,0,30,18,100,0,200,5;endArrowhead=barbed);
    @figure[pli14](type=polyline;coords=10,0,30,18,100,0,200,5;startArrowhead=simple);
    @figure[pli15](type=polyline;coords=10,0,30,18,100,0,200,5;startArrowhead=triangle);
    @figure[pli16](type=polyline;coords=10,0,30,18,100,0,200,5;startArrowhead=barbed);
    @figure[pli17](type=polyline;coords=10,0,30,18,100,0,200,5;startArrowhead=triangle;endArrowhead=triangle);
    @figure[pli18](type=polyline;coords=10,0,30,18,100,0,200,5;startArrowhead=triangle;endArrowhead=barbed); //note: Tk doesn't support having different arrows at the two ends
    //TODO joinstyle, capstyle

    @figure[pli21](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true);
    @figure[pli22](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;lineColor=#00ff00);
    @figure[pli24](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;lineStyle=solid);
    @figure[pli25](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;lineStyle=dashed);
    @figure[pli26](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;lineStyle=dotted);
    @figure[pli27](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;lineWidth=3;lineStyle=solid);
    @figure[pli28](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;lineWidth=3;lineStyle=dashed);
    @figure[pli29](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;lineWidth=3;lineStyle=dotted);
}

