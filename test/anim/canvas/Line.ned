module Line
{
    @class(ArrangeFigures);
    @display("bgb=1000,1000");

    @figure[li01](type=line; points=10,5,200,5);
    @figure[li02](type=line; points=10,5,200,5; lineColor=#00ff00);
    @figure[li04](type=line; points=10,5,200,5; lineStyle=solid);
    @figure[li05](type=line; points=10,5,200,5; lineStyle=dashed);
    @figure[li06](type=line; points=10,5,200,5; lineStyle=dotted);
    @figure[li08](type=line; points=10,5,200,5; lineWidth=3; lineStyle=dashed);
    @figure[li09](type=line; points=10,5,200,5; lineWidth=3; lineStyle=dotted);
    @figure[li10](type=line; points=10,5,200,5; lineWidth=3; lineStyle=dotted);
    @figure[li11](type=line; points=10,5,200,5; endArrowhead=simple);
    @figure[li12](type=line; points=10,5,200,5; endArrowhead=triangle);
    @figure[li13](type=line; points=10,5,200,5; endArrowhead=barbed);
    @figure[li14](type=line; points=10,5,200,5; startArrowhead=simple);
    @figure[li15](type=line; points=10,5,200,5; startArrowhead=triangle);
    @figure[li16](type=line; points=10,5,200,5; startArrowhead=barbed);
    @figure[li17](type=line; points=10,5,200,5; startArrowhead=triangle; endArrowhead=triangle);
    @figure[li18](type=line; points=10,5,200,5; startArrowhead=triangle; endArrowhead=barbed);  //note: Tk doesn't support having different arrows at the two ends
}

