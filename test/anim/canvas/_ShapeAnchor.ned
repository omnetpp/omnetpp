module ShapeAnchor
{
    @display("bgb=800,500");

    @figure[n](type=oval; pos=100,100; size=50,50; anchor=n; fillColor=#a0a0ff);
    @figure[e](type=oval; pos=100,100; size=50,50; anchor=e; fillColor=#a0a0ff);
    @figure[w](type=oval; pos=100,100; size=50,50; anchor=w; fillColor=#a0a0ff);
    @figure[s](type=oval; pos=100,100; size=50,50; anchor=s; fillColor=#a0a0ff);

    @figure[ne](type=oval; pos=200,100; size=50,50; anchor=ne; fillColor=#a0ffa0);
    @figure[nw](type=oval; pos=200,100; size=50,50; anchor=nw; fillColor=#a0ffa0);
    @figure[se](type=oval; pos=200,100; size=50,50; anchor=se; fillColor=#a0ffa0);
    @figure[sw](type=oval; pos=200,100; size=50,50; anchor=sw; fillColor=#a0ffa0);
}

