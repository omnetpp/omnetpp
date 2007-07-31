import "dummy";

module Background
{
    parameters:
        @display("bgb=500,250,yellow,blue,10;bgi=,c;bgg=100,2,red;bgtt=tooltip");
    submodules:
        bgImageFix: BackgroundFixed {
            @display("p=52,35;i=block/blackboard");
        }
        referencePic: Dummy {
            @display("p=457,212;i=device/scanner;bgi=background;bgb=520,290");
        }
        bgImageStretch: BackgroundStretch {
            @display("p=176,35;i=block/blackboard");
        }
        bgImageCenter: BackgroundCenter {
            @display("p=297,35;i=block/blackboard");
        }
        bgImageTile: BackgroundTile {
            @display("p=418,35;i=block/blackboard");
        }
}

module BackgroundFixed
{
    @display("bgi=misc/cloud_vl;bgb=230,120");
    submodules:
        referencePic: Dummy {
            @display("p=162,61;i=device/scanner;bgi=backgroundfixed;bgb=250,160");
        }
}
module BackgroundStretch
{
    @display("bgi=misc/cloud_vl,s;bgb=230,120");
    submodules:
        referencePic: Dummy {
            @display("p=169,71;i=device/scanner;bgi=backgroundstretch;bgb=250,160");
        }
}
module BackgroundCenter
{
    @display("bgi=misc/cloud_vl,c;bgb=230,120");
    submodules:
        referencePic: Dummy {
            @display("p=169,71;i=device/scanner;bgi=backgroundcenter;bgb=250,160");
        }
}
module BackgroundTile
{
    @display("bgi=misc/cloud_vl,t;bgb=230,120");
    submodules:
        referencePic: Dummy {
            @display("p=166,69;i=device/scanner;bgi=backgroundtile;bgb=250,160");
        }
}
