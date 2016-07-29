module Animate
{
    parameters:
        @display("bgb=800,500");

        @figure[txt1](type=text; pos=100,30; text="*** Run the simulation! ***");

        @figure[root](type=rectangle; pos=0,0; size=400,300; transform=translate(100,100); fillColor=#a0a0a0);

        @figure[root.child1](type=rectangle; pos=0,0; size=290,90; transform=translate(10,10); fillColor=#a0a0ff);
        @figure[root.child2](type=rectangle; pos=0,0; size=290,90; transform=translate(10,110); fillColor=#a0ffa0);

        @figure[root.child1.grandchild1](type=rectangle; pos=0,0; size=240,40; transform=translate(10,10); fillColor=#4040ff);
        @figure[root.child1.grandchild2](type=rectangle; pos=0,0; size=240,20; transform=translate(10,60); fillColor=#4040ff);
        @figure[root.child2.grandchild1](type=rectangle; pos=0,0; size=240,40; transform=translate(10,10); fillColor=#40ff40);
        @figure[root.child2.grandchild2](type=rectangle; pos=0,0; size=240,20; transform=translate(10,60); fillColor=#40ff40);

        @figure[oval1](type=oval; pos=400,100; size=200,100; fillColor=#80ff80);

        @figure[root.txt](type=text; pos=100,50; text="text");
        @figure[root.lbl](type=label; pos=200,50; text="label");

        @figure[path1](type=path; path="M 0 0 H 100 V -50"; lineColor=red; lineWidth=3; fillColor=blue; transform=rotate(-15),translate(200,450));
        @figure[path1.label1](type=text; text="This triangle should move at an angle (along its longer side), not horizontally.";  pos=0,0);

    submodules:
       animator: Animator;

}

simple Animator
{
}

