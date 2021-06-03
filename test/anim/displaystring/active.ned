simple Gen
{
    parameters:
        volatile double startTime;
        string what;
    gates:
        output out;
}

simple Sink
{
    gates:
        input in[];
}

module Active
{
    submodules:
        hue: Gen {
            parameters:
                what = "hue";
                startTime = 300;
                @display("p=73,70;b=40,24");
        };
        saturation: Gen {
            parameters:
                what = "saturation";
                startTime = 400;
                @display("p=153,69;b=40,24");
        };
        brightness: Gen {
            parameters:
                what = "brightness";
                startTime = 500;
                @display("p=247,69;b=40,24");
        };
        icons: Gen {
            parameters:
                what = "icons";
                startTime = 0;
                @display("b=40,24;p=211,129");
        };
        boxes: Gen {
            parameters:
                what = "boxes";
                startTime = 100;
                @display("p=208,181;b=40,24");
        };
        dummy1: Dummy {
            parameters:
                @display("p=163,259;i=old/cloud_l");
        };
        options: Gen {
            parameters:
                what = "options";
                startTime = 200;
                @display("p=115,264;b=50,24");
        };
        dummy: Dummy {
            parameters:
                @display("i=old/browser;p=84,249");
        };
        messages: Gen {
            parameters:
                what = "messages";
                startTime = 600;
                @display("p=240,270;i=device/satellite");
        };
        dummy3: Sink {
            parameters:
                @display("p=106,152;b=40,24");
            gates:
                in[6];
        };
    connections allowunconnected:
        icons.out --> dummy3.in[0];
        boxes.out --> dummy3.in[1];
        options.out --> dummy3.in[2];
        messages.out --> dummy3.in[3];
}


