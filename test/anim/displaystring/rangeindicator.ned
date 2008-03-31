module rangeindicator
{
    parameters:
        @display("bgb=430,400;bgg=50,2;bgi=misc/cloud_vl,c");
    submodules:
        referencePic: Dummy {
            @display("p=398,348;i=device/scanner;bgb=434,428,,,0;bgi=rangeindicator");
        }
        range50: Dummy {
            @display("p=51,51;r=50;i=block/dispatch");
        }
        range70: Dummy {
            @display("p=183,51;r=70;i=block/dispatch");
        }
        range90: Dummy {
            @display("p=281,51;r=90;i=block/dispatch");
        }
        rangecdef: Dummy {
            @display("p=51,174;r=50;i=block/dispatch");
        }
        rangecred: Dummy {
            @display("p=131,174;r=50,,red;i=block/dispatch");
        }
        rangechsb: Dummy {
            @display("p=215,174;r=50,,@AAAAAA;i=block/dispatch");
        }
        rangecrgb: Dummy {
            @display("p=305,174;r=50,,#00AA00;i=block/dispatch");
        }
        rangew0: Dummy {
            @display("p=51,258;r=50,,,0;i=block/dispatch");
        }
        rangewdef: Dummy {
            @display("p=131,258;r=50;i=block/dispatch");
        }
        rangew2: Dummy {
            @display("p=215,258;r=50,,,2;i=block/dispatch");
        }
        rangew10: Dummy {
            @display("p=305,258;r=50,,,10;i=block/dispatch");
        }
        rangefilldef: Dummy {
            @display("p=51,348;r=50;i=block/dispatch");
        }
        rangefillred: Dummy {
            @display("p=131,348;r=50,red;i=block/dispatch");
        }
        rangefillhsb: Dummy {
            @display("p=215,348;r=50,@AAAAAA;i=block/dispatch");
        }
        rangefillrgb: Dummy {
            @display("p=305,348;r=50,#00AA00;i=block/dispatch");
        }
}
