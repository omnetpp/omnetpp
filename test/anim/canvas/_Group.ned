module Group
{
    @display("bgb=800,500");

    @figure[top](type=rectangle; pos=0,0; size=400,300; transform=translate(100,100); fillColor=#a0a0a0);

    @figure[top.child1](type=rectangle; pos=0,0; size=290,90; transform=translate(10,10); fillColor=#a0a0ff);
    @figure[top.child2](type=rectangle; pos=0,0; size=290,90; transform=translate(10,110); fillColor=#a0ffa0);

    @figure[top.child1.grandchild1](type=rectangle; pos=0,0; size=240,40; transform=translate(10,10); fillColor=#4040ff);
    @figure[top.child1.grandchild2](type=rectangle; pos=0,0; size=240,20; transform=translate(10,60); fillColor=#4040ff);
    @figure[top.child2.grandchild1](type=rectangle; pos=0,0; size=240,40; transform=translate(10,10); fillColor=#40ff40);
    @figure[top.child2.grandchild2](type=rectangle; pos=0,0; size=240,20; transform=translate(10,60); fillColor=#40ff40);

    @figure[top.child1.grandchild1.text](type=text; pos=0,0; text=One);
    @figure[top.child1.grandchild2.text](type=text; pos=0,0; text=Two);
    @figure[top.child2.grandchild1.text](type=text; pos=0,0; text=Three);
    @figure[top.child2.grandchild2.text](type=text; pos=0,0; text=Four);
}

