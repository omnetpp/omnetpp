import "dummy";

module Icons
{
    submodules:
        icon_large_1: Dummy {
            parameters:
                @display("p=92,84;i=block/sink;is=l;t=note: centers should be aligned\\,\nnot tops or bottoms");
        };
        icon_normal_1: Dummy {
            parameters:
                @display("p=176,84;i=block/sink;is=n");
        };
        icon_small_1: Dummy {
            parameters:
                @display("p=253,84;i=block/sink;is=s");
        };
        icon_verysmall_1: Dummy {
            parameters:
                @display("p=325,84;i=block/sink;is=vs");
        };
        icon_large_2: Dummy {
            parameters:
                @display("p=95,179;i=block/sink_l");
        };
        icon_normal_2: Dummy {
            parameters:
                @display("p=179,179;i=block/sink");
        };
        icon_small_2: Dummy {
            parameters:
                @display("p=256,179;i=block/sink_s");
        };
        icon_verysmall_2: Dummy {
            parameters:
                @display("p=328,179;i=block/sink_vs");
        };
        icon_plus_box: Dummy {
            parameters:
                @display("p=86,261;i=block/sink;is=s");
        };
        icon_plus_icon2: Dummy {
            parameters:
                @display("p=192,260;i=block/sink;is=s;i2=status/asleep");
        };
        box_plus_icon2: Dummy {
            parameters:
                @display("p=294,254;b=30,24,,wheat;i2=status/asleep");
        };
}

