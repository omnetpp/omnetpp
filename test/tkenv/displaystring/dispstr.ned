
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
endmodule

module DisplayStringTest
    submodules:
        hue: Gen;
            parameters:
                what = "hue",
                startTime = 300;
            display: "p=55,52;b=40,24";
        saturation: Gen;
            parameters:
                what = "saturation",
                startTime = 300;
            display: "p=135,51;b=40,24";
        brightness: Gen;
            parameters:
                what = "brightness",
                startTime = 300;
            display: "p=229,51;b=40,24";
        icons: Gen;
            parameters:
                what = "icons",
                startTime = 0;
            display: "p=227,201";
        boxes: Gen;
            parameters:
                what = "boxes",
                startTime = 100;
            display: "p=228,293;b=40,24";
        dummy1: Dummy;
            display: "p=130,299;i=cloud_l";
        options: Gen;
            parameters:
                what = "options",
                startTime = 200;
            display: "p=82,304;b=50,24";
        dummy: Dummy;
            display: "i=browser;p=51,289";
        sink: Sink;
            gatesizes:
                in[6];
            display: "p=85,187;b=40,24";
    connections nocheck:
        hue.out --> sink.in[3];
        saturation.out --> sink.in[4];
        brightness.out --> sink.in[5];

        icons.out --> sink.in[0];
        boxes.out --> sink.in[1];
        options.out --> sink.in[2];
endmodule

network displayStringTest : DisplayStringTest
endnetwork

