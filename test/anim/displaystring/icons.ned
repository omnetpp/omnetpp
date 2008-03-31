module Icons
{
    @display("bgb=623,383");
    submodules:
        rectwithoutsize: Dummy {
            @display("p=270,340;b=,,rect");
        }
        no_box_no_icon: Dummy {
            @display("p=40,340");
        }
        colorized: Dummy {
            @display("p=40,279;i=block/classifier,red");
        }
        referencePic: Dummy {
            @display("p=566,340;i=device/scanner;bgi=icons;bgb=627,410,,,0");
        }
        heightonly: Dummy {
            @display("p=379,36;b=,60;i=block/app2");
        }
        icon_large_1: Dummy {
            parameters:
                @display("p=49,49;i=block/sink;is=l");
        }
        icon_normal_1: Dummy {
            parameters:
                @display("p=133,49;i=block/sink;is=n");
        }
        icon_small_1: Dummy {
            parameters:
                @display("p=210,49;i=block/sink;is=s");
        }
        icon_verysmall_1: Dummy {
            parameters:
                @display("p=282,49;i=block/sink;is=vs");
        }
        icon_large_2: Dummy {
            parameters:
                @display("p=49,133;i=block/sink_l");
        }
        icon_normal_2: Dummy {
            parameters:
                @display("p=133,133;i=block/sink");
        }
        icon_small_2: Dummy {
            parameters:
                @display("p=210,133;i=block/sink_s");
        }
        icon_verysmall_2: Dummy {
            parameters:
                @display("p=282,133;i=block/sink_vs");
        }
        icon_plus_box: Dummy {
            parameters:
                @display("p=49,208;i=block/sink;is=s;b=50,40,rect,wheat");
        }
        icon_plus_icon2: Dummy {
            parameters:
                @display("p=133,208;i=block/sink;is=s;i2=status/stop");
        }
        box_plus_icon2: Dummy {
            parameters:
                @display("p=222,208;b=30,24,,wheat;i2=status/stop");
        }
        widthonly: Dummy {
            @display("p=466,36;b=60;i=block/app2");
        }
        widthandheight: Dummy {
            @display("p=549,36;b=70,50;i=block/app2");
        }
        circlewidth: Dummy {
            @display("p=466,123;b=60,,oval;i=block/circle");
        }
        oval1: Dummy {
            @display("p=549,123;b=70,50,oval;i=block/circle");
        }
        circleheight: Dummy {
            @display("p=379,123;b=,60,oval;i=block/circle");
        }
        fill1: Dummy {
            @display("p=375,190;b=48,28");
        }
        fill2: Dummy {
            @display("p=440,190;b=48,28,,@20A0A0");
        }
        fill3: Dummy {
            @display("p=503,190;b=48,28,,#E02020");
        }
        fill4: Dummy {
            @display("p=568,190;b=48,28,,green");
        }
        bordercol1: Dummy {
            @display("p=373,236;b=48,28");
        }
        bordercol2: Dummy {
            @display("p=438,236;b=48,28,,,@20A0A0");
        }
        bordercol3: Dummy {
            @display("p=501,236;b=48,28,,,#E02020");
        }
        bordercol4: Dummy {
            @display("p=566,236;b=48,28,,,green");
        }
        bwidthdef: Dummy {
            @display("p=373,285;b=48,28");
        }
        bwidth0: Dummy {
            @display("p=438,285;b=48,28,,,,0");
        }
        bwidth1: Dummy {
            @display("p=501,285;b=48,28,,,,1");
        }
        bwidth8: Dummy {
            @display("p=566,285;b=48,28,,,,8");
        }
        colorized50: Dummy {
            @display("p=110,279;i=block/classifier,red,50");
        }
        colorized75: Dummy {
            @display("p=179,279;i=block/classifier,red,75");
        }
        colorized100: Dummy {
            @display("p=249,279;i=block/classifier,red,100");
        }
        unknown_icon: Dummy {
            @display("p=179,340;i=blaa");
        }
        ovalwithoutsize: Dummy {
            @display("p=369,340;b=,,oval");
        }
}

