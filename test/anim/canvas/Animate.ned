module Animate
{
    parameters:
        @display("bgb=800,500");

        @figure[txt1](type=text;coords=100,30;text="*** Run the simulation! ***");

        @figure[root](type=rectangle;coords=100,100,500,400;fillcolor=#a0a0ff);
        @figure[child1](parent=root;type=rectangle;coords=10,10,300,100;fillcolor=#8080ff);
        @figure[child2](parent=root;type=rectangle;coords=10,110,300,200;fillcolor=#8080ff);

        @figure[grandchild1](parent=child1;type=rectangle;coords=10,10,250,50;fillcolor=#6060ff);
        @figure[grandchild2](parent=child1;type=rectangle;coords=10,60,250,80;fillcolor=#6060ff);
        @figure[grandchild3](parent=child2;type=rectangle;coords=10,10,250,50;fillcolor=#6060ff);
        @figure[grandchild4](parent=child2;type=rectangle;coords=10,60,250,80;fillcolor=#6060ff);

        @figure[oval1](type=oval;coords=400,100,600,200;fillcolor=#80ff80);

    submodules:
       animator: Animator;

}

simple Animator
{
}

