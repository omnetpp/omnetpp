module Path
{
    parameters:
        @display("bgb=800,500");

        @figure[p1](type=path; path="M 100 50 L 130 50 L 110 100 Z"; fillColor=cyan);
        @figure[p2](type=path; path="M 200 50 L 230 50 L 210 100"; lineColor=red);
        @figure[p3](type=path; path="M 200 50 L 230 50 L 210 100"; lineColor=red; lineWidth=3; fillColor=blue; transform=translate(80,0),scale(1.5),rotate(20));

        @figure[p4](type=path; path="M 500 100 l 50 0 q 70 -30 50 50 l 0 50 m -50 50 l -50 0 m -50 -50 q -50 -30 -10 -50 t 80 -10 z"; lineColor=darkgreen; lineWidth=7; fillColor=red);

        @figure[p5](type=path; path="M 100 200 Q 280 130 300 200 l 0 100 A 150 50 5 1 1 100 300 Z"; fillColor=yellow; lineWidth=3; lineStyle=dotted);
}

