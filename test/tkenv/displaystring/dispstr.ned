import "colors";

simple Gen
    parameters:
        startTime : numeric,
        what : string;
    gates:
        out: out;
endsimple

simple Sink
    gates:
        in: in[];
endsimple

module Dummy
    gates:
        in: in[];
        out: out[];
    connections nocheck:
endmodule

module DisplayStringTest
    submodules:
        colors: Colors;

        hue: Gen;
            parameters:
                what = "hue",
                startTime = 300;
            display: "p=73,70;b=40,24";
        saturation: Gen;
            parameters:
                what = "saturation",
                startTime = 400;
            display: "p=153,69;b=40,24";
        brightness: Gen;
            parameters:
                what = "brightness",
                startTime = 500;
            display: "p=247,69;b=40,24";
        icons: Gen;
            parameters:
                what = "icons",
                startTime = 0;
            display: "b=40,24;p=211,129";
        boxes: Gen;
            parameters:
                what = "boxes",
                startTime = 100;
            display: "p=208,181;b=40,24";
        dummy1: Dummy;
            display: "p=163,259;i=cloud_l";
        options: Gen;
            parameters:
                what = "options",
                startTime = 200;
            display: "p=115,264;b=50,24";
        dummy: Dummy;
            display: "i=browser;p=84,249";
        dummy3: Sink;
            gatesizes:
                in[6];
            display: "p=106,152;b=40,24";
        icon_large_1: Dummy;
            display: "p=351,75;i=block/sink;is=l;t=note: centers should be aligned\, not tops or bottoms";
        icon_normal_1: Dummy;
            display: "p=435,75;i=block/sink;is=n";
        icon_small_1: Dummy;
            display: "p=512,75;i=block/sink;is=s";
        icon_verysmall_1: Dummy;
            display: "p=584,75;i=block/sink;is=vs";
        icon_large_2: Dummy;
            display: "p=354,170;i=block/sink_l";
        icon_normal_2: Dummy;
            display: "p=438,170;i=block/sink";
        icon_small_2: Dummy;
            display: "p=515,170;i=block/sink_s";
        icon_verysmall_2: Dummy;
            display: "p=587,170;i=block/sink_vs";
        icon_plus_box: Dummy;
            display: "p=345,252;i=block/sink;is=s";
        icon_plus_icon2: Dummy;
            display: "p=451,251;i=block/sink;is=s;i2=status/asleep";
        box_plus_icon2: Dummy;
            display: "p=553,245;b=30,24;o=wheat;i2=status/asleep";
        icon_plus_txt: Dummy;
            display: "p=284,340;i=block/sink;t=some lines\nof text";
        icon_plus_txt_left: Dummy;
            display: "p=585,340;i=block/sink;t=some lines\nof text,l";
        icon_plus_txt_right: Dummy;
            display: "p=378,340;i=block/sink;t=some lines\nof text,r";
        icon_plus_txt_top: Dummy;
            display: "p=488,340;i=block/sink;t=some lines\nof text,t";
        tooltip_1: Dummy;
            display: "p=82,360;b=46,19;tt=This is a\nmulti-line tooltip.";
        tooltip_2: Dummy;
            display: "p=163,362;i=block/network;tt=This is a\nsecond tooltip.";
        conn_plus_txt_1_src: Dummy;
            display: "b=40,24;p=82,439";
        conn_plus_txt_1_dest: Dummy;
            display: "b=40,24;p=231,439";
    connections nocheck:
        icons.out --> dummy3.in[0];
        boxes.out --> dummy3.in[1];
        options.out --> dummy3.in[2];
        conn_plus_txt_1_src.out++ --> conn_plus_txt_1_dest.in++ display "t=some text";
endmodule

network displayStringTest : DisplayStringTest
endnetwork

