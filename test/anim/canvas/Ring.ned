module Ring
{
    @class(ArrangeFigures);
    @arrange(startPos=80,80; lineSpacing=80; columnWidth=80);
    @display("bgb=2300,1000");

    // no innersize
    @figure[ring1](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=);
    @figure[ring2](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=0);
    @figure[ring3](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=0,0);
    @figure[ring4](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=20);
    @figure[ring5](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=,20);
    @figure[ring6](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=20,50);
    @figure[ring7](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=50,20);
    @figure[ring8](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=30,50);
    @figure[ring9](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=30,40);
    @figure[ring10](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=40,30);
    @figure[ring11](type=ring; lineWidth=2; fillColor=green; anchor=center; pos=0,0; size=50,50);


    @addColumn[0](innerSize=);
    @addColumn[1](innerSize=0);
    @addColumn[2](innerSize=0,0);
    @addColumn[3](innerSize=20);
    @addColumn[4](innerSize=,20);
    @addColumn[5](innerSize=20,0);
    @addColumn[6](innerSize=0,20);
    @addColumn[7](innerSize=10,20);
    @addColumn[8](innerSize=20,10);
    @addColumn[9](innerSize=20,20);
    @addColumn[10](innerSize=40,20);
    @addColumn[11](innerSize=20,40);
    @addColumn[12](innerSize=20,50);
    @addColumn[13](innerSize=50,20);
    @addColumn[14](innerSize=50,50);
    @addColumn[15](innerSize=20,60);
    @addColumn[16](innerSize=60,50);
    @addColumn[17](innerSize=60,60);

}

