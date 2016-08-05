module Text
{
    @display("bgb=800,900");
    @class(ArrangeFigures);
    @arrange(lineSpacing=60; columnWidth=300);

    @figure[t01](type=text; pos=50,0; text="Hello World!");
    @figure[t02](type=text; pos=50,0; text="This should be\nmultiline text");
    @figure[t03](type=text; pos=50,0; text="Tabs should work too\nA1\tB1\tC1\nA2\tB2\tC2");

    @figure[t04](type=text; pos=50,0; text="Font face"; font=Times New Roman);
    @figure[t05](type=text; pos=50,0; text="Font size"; font=,23);

    @figure[t06](type=text; pos=50,0; text="Font face and size"; font="Times New Roman",23);

    @figure[t07](type=text; pos=50,0; text="Font style"; font=,,bold);
    @figure[t08](type=text; pos=50,0; text="Font face and style"; font="Times New Roman",,italic);

    @figure[t09](type=text; pos=50,0; text="Font size and style"; font=,23,underline);

    @figure[t10](type=text; pos=50,0; text="Font face and size and style"; font="Times New Roman",23,bold underline italic);

    @figure[t11](type=text; pos=50,0; anchor=nw; text="Font face and size"; font="Times New Roman",3,italic);

    @figure[t12](type=text; pos=50,0; anchor=nw; text="This Is An Item With Many Many Many Many Many Many Many Words");
    @figure[t13](type=text; pos=50,0; anchor=nw; text="This\nIs\nAn\nItem\nWith\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nMany\nLines");
}

