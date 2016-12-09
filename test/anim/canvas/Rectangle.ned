module Rectangle
{
    @class(ArrangeFigures);
    @arrange(startPos=100,100; lineSpacing=70; columnWidth=70);
    @display("bgb=1400,700");

    @addColumn[0](fillColor=green; lineWidth=2);
    @addColumn[1](fillColor=green; lineWidth=2; cornerRadius=0);
    @addColumn[2](fillColor=green; lineWidth=2; cornerRadius=0,0);
    @addColumn[3](fillColor=green; lineWidth=2; cornerRadius=5);
    @addColumn[4](fillColor=green; lineWidth=2; cornerRadius=,5);
    @addColumn[5](fillColor=green; lineWidth=2; cornerRadius=0,5);
    @addColumn[6](fillColor=green; lineWidth=2; cornerRadius=5,0);
    @addColumn[7](fillColor=green; lineWidth=2; cornerRadius=5,5);
    @addColumn[8](fillColor=green; lineWidth=2; cornerRadius=20,10);
    @addColumn[9](fillColor=green; lineWidth=2; cornerRadius=10,20);
    @addColumn[10](fillColor=green; lineWidth=2; cornerRadius=50,20);
    @addColumn[11](fillColor=green; lineWidth=2; cornerRadius=100,10);
    @addColumn[12](fillColor=green; lineWidth=2; cornerRadius=100,100);
    
    @figure[re1](type=rectangle; anchor=center; pos=0,0; size=0,0);
    @figure[re2](type=rectangle; anchor=center; pos=0,0; size=0,10);
    @figure[re3](type=rectangle; anchor=center; pos=0,0; size=10,0);
    @figure[re4](type=rectangle; anchor=center; pos=0,0; size=10,30);
    @figure[re5](type=rectangle; anchor=center; pos=0,0; size=30,10);
    @figure[re6](type=rectangle; anchor=center; pos=0,0; size=40,20);
    @figure[re7](type=rectangle; anchor=center; pos=0,0; size=40,50);
    @figure[re8](type=rectangle; anchor=center; pos=0,0; size=50,50);
}

