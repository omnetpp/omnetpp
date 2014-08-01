module Font
{
    @display("bgb=800,500");
    @class(ArrangeFigures);

    @figure[t01](type=text;coords=50,0;text="Hello World!";anchor=nw;);
    @figure[t02](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial);
    @figure[t03](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Times New Roman,10);
    @figure[t04](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,10);
    @figure[t05](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,12);
    @figure[t06](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,12,normal);
    @figure[t07](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,12,bold);
    @figure[t08](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,12,italic);
    @figure[t09](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,12,underline);
    @figure[t10](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,12,bold underline);
    @figure[t11](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,12,bold underline italic);
    @figure[t12](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Arial,,bold);
    @figure[t13](type=text;coords=50,0;text="Hello World!";anchor=nw;font=Times New Roman,,bold);
    @figure[t14](type=text;coords=50,0;text="Hello World!";anchor=nw;font=,14,);
    @figure[t15](type=text;coords=50,0;text="Hello World!";anchor=nw;font=,,bold);
    @figure[t16](type=text;coords=50,0;text="Hello World!";anchor=nw;font=,14,bold);
}

