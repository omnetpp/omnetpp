module TextAlignment
{
    @display("bgb=1200,250");
    @class(ArrangeFigures);
    @arrange(startPos=70,50; lineSpacing=80; columnWidth=250; drawAnchor=false);

    @addColumn[0](align="left");
    @addColumn[1](align="right");
    @addColumn[2](align="center");

    @figure[t01](type=text; pos=0,0; text="Lorem ipsum\ndolor sit amet,\nconsectetur adipiscing\nelit.");
    @figure[t02](type=label; pos=0,0; text="Lorem ipsum\ndolor sit amet,\nconsectetur adipiscing\nelit.");
}

