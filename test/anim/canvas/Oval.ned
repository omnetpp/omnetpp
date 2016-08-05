module Oval
{
    @class(ArrangeFigures);
    @arrange(startPos=100,100; lineSpacing=70; columnWidth=70);
    @display("bgb=1000,500");

    @addColumn[0](fillColor=green; lineWidth=2);
    @addColumn[1](fillColor=green; lineWidth=2; size=0,);
    @addColumn[2](fillColor=green; lineWidth=2; size=10,);
    @addColumn[3](fillColor=green; lineWidth=2; size=30,);
    @addColumn[4](fillColor=green; lineWidth=2; size=50,);
    
    @figure[ov1](type=oval; anchor=center; pos=0,0);
    @figure[ov2](type=oval; anchor=center; pos=0,0; size=,0);
    @figure[ov3](type=oval; anchor=center; pos=0,0; size=,10);
    @figure[ov4](type=oval; anchor=center; pos=0,0; size=,30);
    @figure[ov5](type=oval; anchor=center; pos=0,0; size=,50);
}

