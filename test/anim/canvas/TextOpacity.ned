module TextOpacity
{
    @display("bgb=1200,250");
    @class(ArrangeFigures);
    @arrange(startPos=70,100; lineSpacing=80; columnWidth=150; drawAnchor=false);

    @addColumn[0](opacity=0);
    @addColumn[1](opacity=0.2);
    @addColumn[2](opacity=0.5);
    @addColumn[3](opacity=0.7);
    @addColumn[4](opacity=1);

    @figure[t01](type=text; pos=0,0; text="Text"; font=,32);
    @figure[t03](type=label; pos=0,0; text="Label"; font=,32);
}

