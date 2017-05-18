module Path
{
    parameters:
        @display("bgb=800,500");

        // reference image
        @figure[ref](type=image; image="path_reference"; pos=300,0; anchor=nw);

        // absolute and relative move and line
        @figure[p0](type=path; path="M 10 10 L 30 10 M 10 30 L 30 30"; offset=100,50;zoomLineWidth=true;lineColor=red);
        @figure[p1](type=path; path="M 10 10 l 0 20 m 20 -20 l 0 20"; offset=150,50;zoomLineWidth=true;lineColor=green);
        
        // absolute and relative horizontal/vertical line
        @figure[p2](type=path; path="M 10 10 H 30 M 10 30 h 20"; offset=100,100;zoomLineWidth=true;lineColor=blue);
        @figure[p3](type=path; path="M 10 10 V 30 M 30 10 v 20"; offset=150,100;zoomLineWidth=true;lineColor=yellow);


        // absolute arc to, rotated, with different color for the 4 flag states
        @figure[p4a](type=path; path="M 10 10 A 20 12 10 0 0 30 30"; offset=50,200;zoomLineWidth=true;lineColor=red;fillColor=red);
        @figure[p4b](type=path; path="M 10 10 A 20 12 10 0 1 30 30"; offset=50,200;zoomLineWidth=true;lineColor=green;fillColor=green);
        @figure[p4c](type=path; path="M 10 10 A 20 12 10 1 0 30 30"; offset=50,200;zoomLineWidth=true;lineColor=blue);
        @figure[p4d](type=path; path="M 10 10 A 20 12 10 1 1 30 30"; offset=50,200;zoomLineWidth=true;lineColor=yellow);

        // with too far end point, should be scaled uniformly.
        @figure[p4ao](type=path; path="M 10 10 A 10 12 10 0 0 30 30"; offset=100,200;zoomLineWidth=true;lineColor=red;fillColor=red);
        @figure[p4bo](type=path; path="M 10 10 A 10 12 10 0 1 30 30"; offset=100,200;zoomLineWidth=true;lineColor=green;fillColor=green);
        @figure[p4co](type=path; path="M 10 10 A 10 12 10 1 0 30 30"; offset=100,200;zoomLineWidth=true;lineColor=blue);
        @figure[p4do](type=path; path="M 10 10 A 10 12 10 1 1 30 30"; offset=100,200;zoomLineWidth=true;lineColor=yellow);

        // relative arc to, rotated, with different color for the 4 flag states
        @figure[p5a](type=path; path="M 10 10 a 20 12 10 0 0 20 20"; offset=150,200;zoomLineWidth=true;lineColor=red;fillColor=red);
        @figure[p5b](type=path; path="M 10 10 a 20 12 10 0 1 20 20"; offset=150,200;zoomLineWidth=true;lineColor=green;fillColor=green);
        @figure[p5c](type=path; path="M 10 10 a 20 12 10 1 0 20 20"; offset=150,200;zoomLineWidth=true;lineColor=blue);
        @figure[p5d](type=path; path="M 10 10 a 20 12 10 1 1 20 20"; offset=150,200;zoomLineWidth=true;lineColor=yellow);

        // with too far end point, should be scaled uniformly.
        @figure[p5ao](type=path; path="M 10 10 a 10 12 10 0 0 20 20"; offset=200,200;zoomLineWidth=true;lineColor=red;fillColor=red);
        @figure[p5bo](type=path; path="M 10 10 a 10 12 10 0 1 20 20"; offset=200,200;zoomLineWidth=true;lineColor=green;fillColor=green);
        @figure[p5co](type=path; path="M 10 10 a 10 12 10 1 0 20 20"; offset=200,200;zoomLineWidth=true;lineColor=blue);
        @figure[p5do](type=path; path="M 10 10 a 10 12 10 1 1 20 20"; offset=200,200;zoomLineWidth=true;lineColor=yellow);


        // absolute and relative quadratic curve and continuation
        @figure[p6](type=path; path="M 20 20 Q 40 0 60 20 T 100 20 T 140 20 t 40 0"; offset=25,250;zoomLineWidth=true;lineColor=green);
        @figure[p7](type=path; path="M 20 20 q 20 -20 40 0 t 40 0 t 40 0 T 180 20"; offset=25,260;zoomLineWidth=true;lineColor=red);

        // absolute and relative cubic curve and continuation
        @figure[p8](type=path; path="M 20 20 C 20 0 60 0 60 20 S 100 40 100 20 S 140 0 140 20 s 40 20 40 0"; offset=25,300;zoomLineWidth=true;lineColor=blue);
        @figure[p9](type=path; path="M 20 20 c 0 -20 40 -20 40 0 s 40 20 40 0 s 40 -20 40 0 S 180 40 180 20"; offset=25,310;zoomLineWidth=true;lineColor=yellow);


        // absolute and relative quadratic curve and continuation, with fallback because there is nothing to continue
        @figure[p10](type=path; path="M 20 20 Q 40 0 60 20 H 100 T 140 20 t 40 0 s 40 -20 40 0"; offset=25,350;zoomLineWidth=true;lineColor=green);
        @figure[p11](type=path; path="M 20 20 q 20 -20 40 0 h 40 t 40 0 T 180 20 S 220 0 220 20"; offset=25,360;zoomLineWidth=true;lineColor=red);

        // absolute and relative cubic curve and continuation, with fallback because there is nothing to continue
        @figure[p12](type=path; path="M 20 20 C 20 0 60 0 60 20 H 100 S 140 0 140 20 s 40 20 40 0 T 220 20"; offset=25,400;zoomLineWidth=true;lineColor=blue);
        @figure[p13](type=path; path="M 20 20 c 0 -20 40 -20 40 0 h 40 s 40 -20 40 0 S 180 40 180 20 t 40 0"; offset=25,410;zoomLineWidth=true;lineColor=yellow);
}

