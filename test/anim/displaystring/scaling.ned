module scaling
{
    parameters:
        @display("bgi=misc/cloud_vl,s;bgb=,,,green4,10;bgg=50,2");
    submodules:
        icon: Dummy {
            @display("p=75.2,24.8;i=block/encrypt;r=20,,,10");
        }
        nosizeinfo: Dummy {
            @display("p=81.6,92;b=,,rect");
        }
        specifiedsizewidth: Dummy {
            @display("p=145.6,62;b=40,24,rect,,,10");
        }
        def: Dummy {
            @display("p=22,48.4");
        }
        referencePic: Dummy {
            @display("p=155,95;i=device/scanner;bgi=scaling;bgb=456,330,,,0");
        }
    connections:
        def.out++ --> icon.in++;
        specifiedsizewidth.out++ --> { @display("ls=,10;t=width10"); } --> nosizeinfo.in++;
        nosizeinfo.out++ --> { @display("ls=,5;t=width5"); } --> def.in++;
        icon.out++ --> specifiedsizewidth.in++;
}
