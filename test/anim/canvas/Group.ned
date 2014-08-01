module Group
{
    @display("bgb=800,500");

    @figure[top](type=rectangle;coords=100,100,500,400;fillColor=#a0a0a0);

    @figure[top.child1](type=rectangle;coords=10,10,300,100;fillColor=#a0a0ff);
    @figure[top.child2](type=rectangle;coords=10,110,300,200;fillColor=#a0ffa0);

    @figure[top.child1.grandchild1](type=rectangle;coords=10,10,250,50;fillColor=#4040ff);
    @figure[top.child1.grandchild2](type=rectangle;coords=10,60,250,80;fillColor=#4040ff);
    @figure[top.child2.grandchild1](type=rectangle;coords=10,10,250,50;fillColor=#40ff40);
    @figure[top.child2.grandchild2](type=rectangle;coords=10,60,250,80;fillColor=#40ff40);

    @figure[top.child1.grandchild1.text](type=text;coords=0,0;text=One);
    @figure[top.child1.grandchild2.text](type=text;coords=0,0;text=Two);
    @figure[top.child2.grandchild1.text](type=text;coords=0,0;text=Three);
    @figure[top.child2.grandchild2.text](type=text;coords=0,0;text=Four);
}

