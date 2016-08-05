module TextFont
{
    @display("bgb=2200,700");
    @class(ArrangeFigures);
    @arrange(startPos=70,50; lineSpacing=80; columnWidth=150; drawAnchor=false);

    @addColumn[a1](font=,12);
    @addColumn[a2](font=,32);
    @addColumn[s1](font=,, italic);
    @addColumn[s2](font=,, bold);
    @addColumn[s3](font=,, underline);
    @addColumn[s4](font=,, italic bold underline);
    @addColumn[b1](font=, 32, italic);
    @addColumn[b2](font=, 32, bold);
    @addColumn[b3](font=, 32, underline);
    @addColumn[b4](font=, 32, italic bold underline);

    @figure[t01](type=text; pos=0,0; text="Text"; font=);
    @figure[t02](type=text; pos=0,0; text="Text"; font=Times New Roman);
    @figure[t03](type=text; pos=0,0; text="Text"; font=Arial);
    @figure[t04](type=text; pos=0,0; text="Text"; font=Courier);
    @figure[l01](type=label; pos=0,0; text="Label"; font=);
    @figure[l02](type=label; pos=0,0; text="Label"; font=Times New Roman);
    @figure[l03](type=label; pos=0,0; text="Label"; font=Arial);
    @figure[l04](type=label; pos=0,0; text="Label"; font=Courier);
}

