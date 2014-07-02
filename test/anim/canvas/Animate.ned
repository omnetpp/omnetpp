module Animate
{
    parameters:
        @display("bgb=800,500");

        @figure[txt1](type=text;coords=100,30;text="*** Run the simulation! ***");

        @figure[root](type=rectangle;coords=100,100,500,400;fillColor=#a0a0a0);

        @figure[root.child1](type=rectangle;coords=10,10,300,100;fillColor=#a0a0ff);
        @figure[root.child2](type=rectangle;coords=10,110,300,200;fillColor=#a0ffa0);

        @figure[root.child1.grandchild1](type=rectangle;coords=10,10,250,50;fillColor=#4040ff);
        @figure[root.child1.grandchild2](type=rectangle;coords=10,60,250,80;fillColor=#4040ff);
        @figure[root.child2.grandchild1](type=rectangle;coords=10,10,250,50;fillColor=#40ff40);
        @figure[root.child2.grandchild2](type=rectangle;coords=10,60,250,80;fillColor=#40ff40);

        @figure[oval1](type=oval;coords=400,100,600,200;fillColor=#80ff80);

    submodules:
       animator: Animator;

}

simple Animator
{
}

