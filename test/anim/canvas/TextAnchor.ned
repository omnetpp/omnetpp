module TextAnchor
{
    @display("bgb=2500,500");
    @class(ArrangeFigures);
    @arrange(startPos=50,100; lineSpacing=100; columnWidth=150);

    @addColumn[0](anchor=c);
    @addColumn[1](anchor=n);
    @addColumn[2](anchor=nw);
    @addColumn[3](anchor=w);
    @addColumn[4](anchor=sw);
    @addColumn[5](anchor=s);
    @addColumn[6](anchor=se);
    @addColumn[7](anchor=e);
    @addColumn[8](anchor=ne);
    @addColumn[9](anchor=start);
    @addColumn[10](anchor=middle);
    @addColumn[11](anchor=end);

    @figure[t01](type=text; pos=0,0; text="Text figure");
    @figure[t02](type=text; pos=0,0; text="Multi-\nline\ntext figure");
    @figure[t03](type=label; pos=0,0; text="Label figure");
    @figure[t04](type=label; pos=0,0; text="Multi-\nline\nlabel figure");
}

