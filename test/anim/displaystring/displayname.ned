module DisplayName
{
    @display("bgb=400,600");
    submodules:
        node[6] : Dummy { // note: display names are configured in omnetpp.ini
            @display("p=100,50,column");
        }

        referencePic: Dummy {
            @display("p=250,50;i=device/scanner;bgi=displayname;bgb=405,606,,0");
        }
}

