module TextColor
{
    @display("bgb=1200,200");
    @class(ArrangeFigures);
    @arrange(startPos=70,50; lineSpacing=80; columnWidth=150; drawAnchor=false);

    @addColumn[0](color=red);
    @addColumn[1](color=green);
    @addColumn[2](color=white);
    @addColumn[3](color=black);

    @figure[t01](type=text; pos=0,0; text="Text figure");
    @figure[t03](type=label; pos=0,0; text="Label figure");
}

