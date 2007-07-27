import "dummy";

module Background
{
    parameters:
        @display("bgb=500,500,yellow,blue,10");
    submodules:
        icon1: Dummy {
            parameters:
                @display("p=100,80;i=block/sink;t=bg should be 500x500 yellow\nw/ thick blue border");
        };
}

