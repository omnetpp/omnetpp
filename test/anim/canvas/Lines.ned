module Lines
{
    @class(ArrangeFigures);
    @display("bgb=1000,1000");

    @figure[li01](type=line;coords=10,5,200,5);
    @figure[li02](type=line;coords=10,5,200,5;linecolor=#00ff00);
    @figure[li04](type=line;coords=10,5,200,5;linestyle=solid);
    @figure[li05](type=line;coords=10,5,200,5;linestyle=dashed);
    @figure[li06](type=line;coords=10,5,200,5;linestyle=dotted);
    @figure[li08](type=line;coords=10,5,200,5;linewidth=3;linestyle=dashed);
    @figure[li09](type=line;coords=10,5,200,5;linewidth=3;linestyle=dotted);
    @figure[li10](type=line;coords=10,5,200,5;linewidth=3;linestyle=dotted);
    @figure[li11](type=line;coords=10,5,200,5;endarrowhead=simple);
    @figure[li12](type=line;coords=10,5,200,5;endarrowhead=triangle);
    @figure[li13](type=line;coords=10,5,200,5;endarrowhead=barbed);
    @figure[li14](type=line;coords=10,5,200,5;startarrowhead=simple);
    @figure[li15](type=line;coords=10,5,200,5;startarrowhead=triangle);
    @figure[li16](type=line;coords=10,5,200,5;startarrowhead=barbed);
    @figure[li17](type=line;coords=10,5,200,5;startarrowhead=triangle;endarrowhead=triangle);
    @figure[li18](type=line;coords=10,5,200,5;startarrowhead=triangle;endarrowhead=barbed); //note: Tk doesn't support having different arrows at the two ends

    @figure[pli01](type=polyline;coords=10,0,30,18,100,0,200,5);
    @figure[pli02](type=polyline;coords=10,0,30,18,100,0,200,5;linecolor=#00ff00);
    @figure[pli04](type=polyline;coords=10,0,30,18,100,0,200,5;linestyle=solid);
    @figure[pli05](type=polyline;coords=10,0,30,18,100,0,200,5;linestyle=dashed);
    @figure[pli06](type=polyline;coords=10,0,30,18,100,0,200,5;linestyle=dotted);
    @figure[pli10](type=polyline;coords=10,0,30,18,100,0,200,5;linewidth=3;linestyle=solid);
    @figure[pli08](type=polyline;coords=10,0,30,18,100,0,200,5;linewidth=3;linestyle=dashed);
    @figure[pli09](type=polyline;coords=10,0,30,18,100,0,200,5;linewidth=3;linestyle=dotted);
    @figure[pli11](type=polyline;coords=10,0,30,18,100,0,200,5;endarrowhead=simple);
    @figure[pli12](type=polyline;coords=10,0,30,18,100,0,200,5;endarrowhead=triangle);
    @figure[pli13](type=polyline;coords=10,0,30,18,100,0,200,5;endarrowhead=barbed);
    @figure[pli14](type=polyline;coords=10,0,30,18,100,0,200,5;startarrowhead=simple);
    @figure[pli15](type=polyline;coords=10,0,30,18,100,0,200,5;startarrowhead=triangle);
    @figure[pli16](type=polyline;coords=10,0,30,18,100,0,200,5;startarrowhead=barbed);
    @figure[pli17](type=polyline;coords=10,0,30,18,100,0,200,5;startarrowhead=triangle;endarrowhead=triangle);
    @figure[pli18](type=polyline;coords=10,0,30,18,100,0,200,5;startarrowhead=triangle;endarrowhead=barbed); //note: Tk doesn't support having different arrows at the two ends
    //TODO joinstyle, capstyle

    @figure[pli21](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true);
    @figure[pli22](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;linecolor=#00ff00);
    @figure[pli24](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;linestyle=solid);
    @figure[pli25](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;linestyle=dashed);
    @figure[pli26](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;linestyle=dotted);
    @figure[pli27](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;linewidth=3;linestyle=solid);
    @figure[pli28](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;linewidth=3;linestyle=dashed);
    @figure[pli29](type=polyline;coords=10,0,30,18,100,0,200,5;smooth=true;linewidth=3;linestyle=dotted);
}

