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

    @figure[l06](type=label; pos=100,0; text="This label is rotated 45 degrees left"; angle=45);
    @figure[l07](type=label; pos=100,0; text="This label is rotated 90 degrees right"; angle=-90);

    @figure[l08](type=label; pos=150,0; anchor=c; text="This label is rotated 30 degrees right"; angle=-30);
}

