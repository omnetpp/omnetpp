import "dummy";

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

module Active
    submodules:
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
    connections nocheck:
        icons.out --> dummy3.in[0];
        boxes.out --> dummy3.in[1];
        options.out --> dummy3.in[2];
endmodule


