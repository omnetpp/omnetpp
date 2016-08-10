module AssociatedObject {
    parameters:
        @class(AssociateObjects);
        @display("bgb=800,800");


        @figure[t1](type=text; pos=25,25; font=,11; text="Rectangles with red/green/blue outline have the submodules a/b/c associated with them respectively.\nClicking on a figure or any of its children should \"select\" (show in the inspector)\nthe associated object instead of the figure itself.\nThe context menu of these figures should contain both the associated object and the figure itself.\nThe smaller rectangles are all child figures of the rectangles directly behind them.");

        @figure[a1]       (type=rectangle; pos=200,200; anchor=center; size=120,120; fillColor=grey100; lineWidth=5; lineColor=red);
        @figure[a1.a2]    (type=rectangle; pos=200,200; anchor=center; size=80,80; fillColor=grey75);
        @figure[a1.a2.a3] (type=rectangle; pos=200,200; anchor=center; size=40,40; fillColor=grey50);

        @figure[b1]       (type=rectangle; pos=400,200; anchor=center; size=120,120; fillColor=grey100);
        @figure[b1.b2]    (type=rectangle; pos=400,200; anchor=center; size=80,80; fillColor=grey75; lineWidth=5; lineColor=green);
        @figure[b1.b2.b3] (type=rectangle; pos=400,200; anchor=center; size=40,40; fillColor=grey50);

        @figure[c1]       (type=rectangle; pos=600,200; anchor=center; size=120,120; fillColor=grey100);
        @figure[c1.c2]    (type=rectangle; pos=600,200; anchor=center; size=80,80; fillColor=grey75);
        @figure[c1.c2.c3] (type=rectangle; pos=600,200; anchor=center; size=40,40; fillColor=grey50; lineWidth=5; lineColor=blue);

        @figure[w1]       (type=rectangle; pos=400,420; anchor=center; size=120,120; fillColor=grey100; lineWidth=5; lineColor=red);
        @figure[w1.w2]    (type=rectangle; pos=400,420; anchor=center; size=80,80; fillColor=grey75; lineWidth=5; lineColor=green);
        @figure[w1.w2.w3] (type=rectangle; pos=400,420; anchor=center; size=40,40; fillColor=grey50; lineWidth=5; lineColor=blue);
        
        
        @figure[t2](type=text; pos=25,525; font=,11; text="These figures are associated to each other.\nClicking on any of them should select the other one.\nThe context menu of each should contain both of them.");
        
        @figure[x1](type=rectangle; pos=300,700; anchor=center; size=100,100; fillColor=black; lineWidth=5; lineColor=white);
        @figure[x2](type=oval;      pos=500,700; anchor=center; size=100,100; fillColor=white; lineWidth=5; lineColor=black);

    types:
        module Dummy { }

    submodules:
        a: Dummy { @display("p=200,300;i=,red,75"); }
        b: Dummy { @display("p=400,300;i=,green,75"); }
        c: Dummy { @display("p=600,300;i=,blue,75"); }
}
