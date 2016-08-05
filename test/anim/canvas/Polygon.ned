module Polygon
{
    @class(ArrangeFigures);
    @arrange(startPos=50,50; lineSpacing=70; columnWidth=70);
    @display("bgb=1000,600");

    @addColumn[0](smooth=false; fillRule=nonzero; fillColor=green; lineWidth=2);
    @addColumn[1](smooth=false; fillRule=evenodd; fillColor=green; lineWidth=2);

    @addColumn[2](smooth=true; fillRule=nonzero; fillColor=green; lineWidth=2);
    @addColumn[3](smooth=true; fillRule=evenodd; fillColor=green; lineWidth=2);


    @figure[pgn00](type=polygon; points=0,0, 40,0, 50,50);
    @figure[pgn01](type=polygon; points=0,0, 0,40, 50,50);

    @figure[pgn02](type=polygon; points=0,0, 20,-10, 40,0, 50,50);
    @figure[pgn03](type=polygon; points=0,0, 20,-10, 0,40, 50,50);

    @figure[pgn04](type=polygon; points=0,0, 40,40, 10,40, 50,0, 50,50, 0,50);
    @figure[pgn05](type=polygon; points=0,0, 40,40, 40,10, 0,50, 50,50, 50,0);

    @figure[pgn06](type=polygon; points=5,0, 50,0, 50,50, 0,50, 0,10, 40,10, 40,40, 10,40, 10,20, 30,20, 30,30, 20,30, 5,20);
    @figure[pgn07](type=polygon; points=0,5, 0,50, 50,50, 50,0, 10,0, 10,40, 40,40, 40,10, 20,10, 20,30, 30,30, 30,20, 20,5);
}

