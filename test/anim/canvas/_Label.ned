module Label
{
    @display("bgb=800,500");
    @class(ArrangeFigures);
    @arrange(lineSpacing=60);

    @figure[t01](type=label; pos=50,0; anchor=nw; text="Hello World!");
    @figure[t02](type=label; pos=50,0; anchor=nw; text="This should be\nmultiline text");
    @figure[t03](type=label; pos=50,0; anchor=nw; text="Tabs should work too\nA1\tB1\tC1\nA2\tB2\tC2");
    @figure[t04](type=label; pos=50,0; anchor=nw; text="Font face"; font=Times New Roman);
    @figure[t05](type=label; pos=50,0; anchor=nw; text="Font face and size"; font=Times New Roman,10);
}

