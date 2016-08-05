module Line
{
    @class(ArrangeFigures);
    @arrange(startPos=100,50; lineSpacing=100; columnWidth=100);
    @display("bgb=500,2200");

    @figure[li01](type=line; points=0,0,50,0);
    @figure[li02](type=line; points=0,0,50,0.5);
    @figure[li03](type=line; points=0,0.5,50,0.5);
    @figure[li04](type=line; points=0,-0.5,50,0.5);
    @figure[li05](type=line; points=0,-1,50,1);
    
    @figure[li06](type=line; points=0,0,0,50);
    @figure[li07](type=line; points=0.5,0,0.5,50);
    @figure[li08](type=line; points=-0.5,0,-0.5,50);
    @figure[li09](type=line; points=-0.5,0,0.5,50);
    @figure[li10](type=line; points=-1,0,1,50);
    
    @figure[li11](type=line; points=0,0,50,50);
    @figure[li12](type=line; points=50,0,0,0);
    @figure[li13](type=line; points=0,50,0,0);
    @figure[li14](type=line; points=50,50,0,0);
    @figure[li15](type=line; points=-50,0,0,0);
    @figure[li16](type=line; points=0,-50,0,0);
    @figure[li17](type=line; points=-50,-50,0,0);
    @figure[li18](type=line; points=-50,-50,50,50);
    
    @figure[li19](type=line; points=0,0,0,0);
    @figure[li20](type=line; points=0,0,1,0);
    @figure[li21](type=line; points=0,0,0,1);
    @figure[li22](type=line; points=20,10,20,10);
}

