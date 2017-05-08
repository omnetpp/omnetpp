module Panel
{
    parameters:
        @display("bgb=800,500");

        @figure[root1](type=rectangle; pos=50,50; size=200,200);

        @figure[root1.base](type=rectangle; pos=100,100; size=100,100; fillColor=white);

        @figure[root1.nw](type=panel; pos=100,100; anchorPoint=0,0);
        @figure[root1.ne](type=panel; pos=200,100; anchorPoint=100,0);
        @figure[root1.se](type=panel; pos=200,200; anchorPoint=100,100);
        @figure[root1.sw](type=panel; pos=100,200; anchorPoint=0,100);
        @figure[root1.c](type=panel; pos=150,150; anchorPoint=50,50);


        @figure[root1.nw.r](type=rectangle; pos=0,0; size=100,100; fillColor=red);
        @figure[root1.ne.r](type=rectangle; pos=0,0; size=100,100; fillColor=green);
        @figure[root1.se.r](type=rectangle; pos=0,0; size=100,100; fillColor=blue);
        @figure[root1.sw.r](type=rectangle; pos=0,0; size=100,100; fillColor=yellow);
        @figure[root1.c.r](type=rectangle; pos=0,0; size=100,100; fillColor=purple);



        @figure[root2](type=rectangle; pos=50,50; size=200,200; transform=translate(100,100),rotate(30,20,20),skewx(0.7,60),translate(150,-80));

        @figure[root2.base](type=rectangle; pos=100,100; size=100,100; fillColor=white);

        // these translations should be ineffective due to the fact that the anchorPoints are translated as well
        @figure[root2.nw](type=panel; pos=100,100; anchorPoint=0,0; transform=translate(20,0));
        @figure[root2.ne](type=panel; pos=200,100; anchorPoint=100,0; transform=translate(-20,100));
        @figure[root2.se](type=panel; pos=200,200; anchorPoint=100,100; transform=translate(1000,100));
        @figure[root2.sw](type=panel; pos=100,200; anchorPoint=0,100; transform=translate(0,100));
        @figure[root2.c](type=panel; pos=150,150; anchorPoint=50,50; transform=translate(-20,50));


        @figure[root2.nw.r](type=rectangle; pos=0,0; size=100,100; transform=rotate(-20,0,0),scale(0.5); fillColor=red);
        @figure[root2.ne.r](type=rectangle; pos=0,0; size=100,100; transform=rotate(-20,100,0),scale(0.5,100,0); fillColor=green);
        @figure[root2.se.r](type=rectangle; pos=0,0; size=100,100; transform=rotate(-20,100,100),scale(0.5,100,100); fillColor=blue);
        @figure[root2.sw.r](type=rectangle; pos=0,0; size=100,100; transform=rotate(-20,0,100),scale(0.5,0,100); fillColor=yellow);
        @figure[root2.c.r](type=rectangle; pos=0,0; size=100,100; transform=rotate(-20,50,50),scale(0.5,50,50); fillColor=purple);
}
