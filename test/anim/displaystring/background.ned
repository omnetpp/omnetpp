import "dummy";

module Background
{
    parameters:
        @display("bgb=500,500,,blue,10;bgi=misc/cloud_vl,c;bgg=100,2;bgtt=tooltip");
    submodules:
        icon1: Dummy {
            parameters:
                @display("p=100,80;i=block/sink;t=bg should be 500x500 yellow\nw/ thick blue border");
        }
}

