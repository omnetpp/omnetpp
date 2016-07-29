module Text
{
    @display("bgb=800,500");
    @class(ArrangeFigures);

    @figure[t01](type=text; pos=50,0; anchor=nw; text="Hello World!");
    @figure[t02](type=text; pos=50,0; anchor=nw; text="This should be\nmultiline text");
    @figure[t03](type=text; pos=50,0; anchor=nw; text="Tabs should work too\nA\tB\tC\n123\t22\t3344\t\n\t4\t556");
    @figure[t04](type=text; pos=50,0; anchor=nw; text="Font face"; font=Times New Roman);
    @figure[t05](type=text; pos=50,0; anchor=nw; text="Font face and size"; font=Times New Roman,10);
}

