module ZOrder
{
    @display("bgb=1200,1200");

    @figure[t0] (type=text;text=Figures should appear cascaded;coords=100,100);
    @figure[o3] (type=rectangle;fillcolor=#40ff40;coords=300,300;size=500,500;anchor=nw;z=.3);
    @figure[o3a](type=rectangle;fillcolor=#40ff40;coords=330,330;size=500,500;anchor=nw;z=.3);
    @figure[o3b](type=rectangle;fillcolor=#40ff40;coords=360,360;size=500,500;anchor=nw;z=.3);
    @figure[o4] (type=rectangle;fillcolor=#60ff60;coords=400,400;size=500,500;anchor=nw;z=.4);
    @figure[o1] (type=rectangle;fillcolor=#00ff00;coords=500,500;size=500,500;anchor=nw;z=.1);
    @figure[o5] (type=rectangle;fillcolor=#80ff80;coords=500,500;size=500,500;anchor=nw;z=.5);
    @figure[o2] (type=rectangle;fillcolor=#20ff20;coords=200,200;size=500,500;anchor=nw;z=.2);
}

