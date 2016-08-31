module TextHalo
{
    @display("bgb=1200,200;bgi=background/green,t");
    @class(ArrangeFigures);
    @arrange(startPos=70,50; lineSpacing=80; columnWidth=150; drawAnchor=false);

    @addColumn[0](halo=false);
    @addColumn[1](halo=true);
    @addColumn[2](halo=false;color=red);
    @addColumn[3](halo=true;color=red);

    @figure[t01](type=text; pos=0,0; text="Text");
    @figure[t03](type=label; pos=0,0; text="Label");
}

