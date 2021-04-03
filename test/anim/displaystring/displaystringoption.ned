module DisplayStringOption
{
    @display("bgb=400,200");
    submodules:
        node[4] : Dummy {
            // note: additional display strings are configured in omnetpp.ini
            @display("p=100,80,row;i=block/cogwheel,red,50");
        }
        referencePic: Dummy {
            @display("p=350,150;i=device/scanner;bgi=displaystringoption;bgb=404,204,0,0");
        }
}

