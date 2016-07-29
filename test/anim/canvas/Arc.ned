module Arc
{
    @class(ArrangeFigures);
    @display("bgb=1000,1000");

    @figure[ar1](type=arc; pos=10,0; size=40,10; startAngle=0; endAngle=250);
    @figure[ar3](type=arc; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineColor=#ff0000; startArrowhead=triangle);
    @figure[ar4](type=arc; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineWidth=3);
    @figure[ar5](type=arc; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineStyle=solid; startArrowhead=barbed; endArrowhead=simple);
    @figure[ar6](type=arc; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineStyle=dotted; startArrowhead=simple; endArrowhead=triangle);
    @figure[ar7](type=arc; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineStyle=dashed; startArrowhead=triangle; endArrowhead=barbed);
    @figure[ar8](type=arc; pos=10,0; size=40,10; startAngle=0; endAngle=250; lineColor=#ff0000; lineWidth=3; lineStyle=dashed);
}

