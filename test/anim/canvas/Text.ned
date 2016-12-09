module Text
{
    @display("bgb=1200,600");
    @class(ArrangeFigures);
    @arrange(startPos=0,50;lineSpacing=60; columnWidth=450);

    @figure[t01](type=text; pos=50,0; text="Hello World!");
    @figure[t02](type=text; pos=50,0; text="This should be\nmultiline text");
    @figure[t03](type=text; pos=50,0; text="Tabs should work too\nA1\tB1\tC1\nA2\tB2\tC2");

    @figure[t04](type=text; pos=50,0; text="This Is An Item With Many Many Many Many Many Many Words");
    @figure[t05](type=text; pos=50,0; text="This\nIs\nAn\nItem\nWith\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nLines");
}

