module Label
{
    @display("bgb=1200,600");
    @class(ArrangeFigures);
    @arrange(startPos=0,50;lineSpacing=60; columnWidth=450);

    @figure[l01](type=label; pos=50,0; text="Hello World!");
    @figure[l02](type=label; pos=50,0; text="This should be\nmultiline text");
    @figure[l03](type=label; pos=50,0; text="Tabs should work too\nA1\tB1\tC1\nA2\tB2\tC2");

    @figure[l04](type=label; pos=50,0; text="This Is An Item With Many Many Many Many Many Many Words");
    @figure[l05](type=label; pos=50,0; text="This\nIs\nAn\nItem\nWith\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nLines");
}

