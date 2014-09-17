module Animate
{
    parameters:
        @display("bgb=800,500");

        @figure[txt1](type=text;position=100,30;text="*** Run the simulation! ***");

        @figure[root](type=rectangle;position=0,0;size=400,300;transform=translate(100,100);fillColor=#a0a0a0);

        @figure[root.child1](type=rectangle;position=0,0;size=290,90;transform=translate(10,10);fillColor=#a0a0ff);
        @figure[root.child2](type=rectangle;position=0,0;size=290,90;transform=translate(10,110);fillColor=#a0ffa0);

        @figure[root.child1.grandchild1](type=rectangle;position=0,0;size=240,40;transform=translate(10,10);fillColor=#4040ff);
        @figure[root.child1.grandchild2](type=rectangle;position=0,0;size=240,20;transform=translate(10,60);fillColor=#4040ff);
        @figure[root.child2.grandchild1](type=rectangle;position=0,0;size=240,40;transform=translate(10,10);fillColor=#40ff40);
        @figure[root.child2.grandchild2](type=rectangle;position=0,0;size=240,20;transform=translate(10,60);fillColor=#40ff40);

        @figure[oval1](type=oval;position=400,100;size=200,100;fillColor=#80ff80);

        @figure[root.txt](type=text;position=100,50;text="text");
        @figure[root.lbl](type=label;position=200,50;text="label");

        @figure[path1](type=path;path="M 0 0 L 30 0 L 10 50";lineColor=red;lineWidth=3;fillColor=blue;transform=translate(280,100),scale(1.5),rotate(40));
        @figure[path1.label1](type=label;text="This should move at an angle, not horizontally."; position=10,10);

    submodules:
       animator: Animator;

}

simple Animator
{
}

