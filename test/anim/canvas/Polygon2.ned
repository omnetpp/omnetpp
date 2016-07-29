module Polygon2
{
    @display("bgb=1000,1200");

    @figure[lbl1](type=label;  pos=100,80;  text="fillRule=evenodd vs nonzero");
    @figure[pgn1](type=polygon;  points=0,0, 0,200, 200,200, 200,0, 50,100, 150,100;  fillColor=lightblue;  fillRule=evenodd;  transform=translate(100,100));
    @figure[pgn2](type=polygon;  points=0,0, 0,200, 200,200, 200,0, 50,100, 150,100;  fillColor=lightblue;  fillRule=nonzero;  transform=translate(400,100));
}

