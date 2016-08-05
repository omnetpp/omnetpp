module Polyline
{
    @class(ArrangeFigures);
    @arrange(startPos=100,100; lineSpacing=100; columnWidth=100; addReferenceLine=true);
    @display("bgb=1200,2300");
    
    @addColumn[0](smooth=false; lineWidth=4);
    @addColumn[1](smooth=true; lineWidth=4);

    @figure[pli01](type=polyline; points=0,0,50,0);
    @figure[pli02](type=polyline; points=0,0,0,50);
    @figure[pli03](type=polyline; points=0,50,0,0);
    @figure[pli04](type=polyline; points=50,0,0,0);
    @figure[pli05](type=polyline; points=-20,20,20,-20);
    @figure[pli06](type=polyline; points=10,10,10,10);

    @figure[pli07](type=polyline; points=0,0,0,25,50,25);
    @figure[pli08](type=polyline; points=0,0,0,25,50,25,50,50);
    @figure[pli09](type=polyline; points=0,0,0,25,50,25,50,50,0,50);
    @figure[pli10](type=polyline; points=0,0,0,25,50,25,50,50,0,50,0,75);
    @figure[pli11](type=polyline; points=0,0,0,20,0,25,5,25,45,25,50,25,50,50,0,50,0,55,0,75);
    
    @figure[pli12](type=polyline; points=-10,0,0,50,10,0);
    @figure[pli13](type=polyline; points=10,0,0,50,-10,0);
    @figure[pli14](type=polyline; points=-10,0,0,50,0,50,10,0);
    @figure[pli15](type=polyline; points=-10,0,0,50,0,50,0,50,10,0);
    
    @figure[pli16](type=polyline; points=0,0,0,50,0,0);
    @figure[pli17](type=polyline; points=0,0,0,50,0,50,0,0);
    @figure[pli18](type=polyline; points=0,0,50,0,0,0);
    @figure[pli19](type=polyline; points=0,0,50,0,50,0,0,0);

    @figure[pli20](type=polyline; points=0,0,0,0,50,40; startArrowhead=simple; endArrowhead=simple);    
    @figure[pli21](type=polyline; points=0,0,0,0,50,40,50,40; startArrowhead=simple; endArrowhead=simple);
    @figure[pli22](type=polyline; points=0,0,25,20,25,20,50,40; startArrowhead=simple; endArrowhead=simple);
}

