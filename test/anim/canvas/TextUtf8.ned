module TextUtf8
{
    @display("bgb=1200,250");
    @class(ArrangeFigures);
    @arrange(startPos=70,50; lineSpacing=80; columnWidth=250; drawAnchor=false);

    @addColumn[0](font=,16);
    @addColumn[1](font=Arial,16);

    @figure[t01](type=text; pos=0,0; text="HU: Cipőfűző\nJP: こんにちは\nSymbols: ☀☁☕☢");
    @figure[t02](type=label; pos=0,0; text="HU: Cipőfűző\nJP: こんにちは\nSymbols: ☀☁☕☢");
}

